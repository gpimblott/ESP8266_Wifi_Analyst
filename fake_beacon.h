#ifndef FAKE_BEACON_H
#define FAKE_BEACON_H

#include "esp_functions.h"

char fake_beacon_ssid[14][MAX_SSID_LEN];

/*
 *  Send fake beacon packets.
 */
uint8_t beacon_packet[128] = {
  0x80, 0x00, // Frame Control
  0x00, 0x00, // duration
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination Address
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source Address - Overwritten later
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // BSSID - overwritten to the same as source
  0xc0, 0x6c, // Seq-ctl
  // frame body starts here
  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, // Timestamp
  0x64, 0x00, // Beacon interval
  0x01, 0x04, // Capability info
  // SSID
  /* 36 */
  0x00
};

void fake_beacon(char *ssid, uint8_t packets, byte channel) {
  if (strlen(ssid) > 0 && packets > 0) {
    beacon_packet[10] = beacon_packet[16] = random(256);
    beacon_packet[11] = beacon_packet[17] = random(256);
    beacon_packet[12] = beacon_packet[18] = random(256);
    beacon_packet[13] = beacon_packet[19] = random(256);
    beacon_packet[14] = beacon_packet[20] = random(256);
    beacon_packet[15] = beacon_packet[21] = random(256);

    int ssidLen = strlen(ssid);
    beacon_packet[37] = ssidLen;
    for (int i = 0; i < ssidLen; i++) {
      beacon_packet[38 + i] = ssid[i];
    }

    uint8_t postSSID[13] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, //supported rate
                            0x03, 0x01, 0x04 /*DSSS (Current Channel)*/
                           };

    for (int i = 0; i < 12; i++) {
      beacon_packet[38 + ssidLen + i] = postSSID[i];
    }

    beacon_packet[50 + ssidLen] = channel;

    int packetSize = 51 + ssidLen;
    for (uint8_t i = 0; i < packets; i++) {
      wifi_send_pkt_freedom(beacon_packet, packetSize, 0);
    }
  }
}



#endif
