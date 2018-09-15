
#include "clients.h"

Clients::Clients() {
  
}



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
