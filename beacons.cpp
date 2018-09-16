#include "beacons.h"


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


Beacons::Beacons() {
  beacons_count = 0;
  beacons_index = 0;
}


/*
 *  Function that parses beacon information from frame
 */
struct beaconinfo Beacons::parse(uint8_t *frame, uint16_t framelen, signed rssi) {
  struct beaconinfo bi;
 
  bi.ssid_len = 0;
  bi.channel = 0;
  bi.err = 0;
  bi.rssi = -rssi;
  
  int pos = 36;
  if (frame[pos] == 0x00) {
    while (pos < framelen) {
      
      switch (frame[pos]) {
        case 0x00: //SSID
          bi.ssid_len = (int) frame[pos + 1];
          
          if (bi.ssid_len == 0) {
            memset(bi.ssid, '\x00', 33);
            break;
          }
          if ((bi.ssid_len < 0) || (bi.ssid_len > 32)) {
            bi.err = 1;
            break;
          }
        
          memset(bi.ssid, '\x00', 33);
          memcpy(bi.ssid, frame + pos + 2, bi.ssid_len);
          bi.err = 0;
          break;
        
        case 0x03: //Channel
          bi.channel = (int) frame[pos + 2];
          pos = -1;
          break;
        
        default:
          break;
      }
      if (pos < 0) break;
      pos += (int) frame[pos + 1] + 2;
    }
  } else {
    bi.err = 1;
  }
  memcpy(bi.beacon, frame + 10, ETH_MAC_LEN);
  return bi;
}

/*
 *  Function that stores information about single beacon
 */
boolean Beacons::store(beaconinfo bi) {
  boolean known = false;
  int u;
  
  for (u = 0; u < beacons_count; u++) {
    if (!memcmp(beacons_known[u].beacon, bi.beacon, ETH_MAC_LEN)) {
      known = true;
      break;
    }
  }
  
  if (known) {
    memcpy(&beacons_known[u], &bi, sizeof(bi));
  } else {
    memcpy(&beacons_known[beacons_index], &bi, sizeof(bi));
    
    if (beacons_count < MAX_BEACONS) beacons_count++;
    beacons_index++;
    
    if (beacons_index == MAX_BEACONS) beacons_index = 0;
  }
  return known;
}

/**
 * Add a new fake SSID
 */
void Beacons::addFake(int channel, char *ssid) {
  Serial.printf("Adding fake beacon: %d %s\n" , channel , ssid);
  
  memset(fake_beacon_ssid[channel - 1], 0, MAX_SSID_LEN);
  strncpy(fake_beacon_ssid[channel - 1], ssid, MAX_SSID_LEN);
}

/**
 * Send a fake AP packet
 */
void Beacons::sendFake(byte channel, uint8_t packets) {
  char * ssid = fake_beacon_ssid[channel - 1];

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


/*
 *  Function that prints single beacon in JSON format
 */
void Beacons::print(beaconinfo bi) {
  Serial.print("\"");
  for (int i = 0; i < ETH_MAC_LEN; i++) {
    if (i > 0) Serial.print(":");
    Serial.printf("%02x", bi.beacon[i]);
  }
  Serial.printf("\":{\"channel\":%d,\"rssi\":-%d,\"ssid\":\"%s\"}", bi.channel, bi.rssi, bi.ssid);
}


/*
 *   Function that prints all beacons in JSON format
 */
void Beacons::print() {
  Serial.print("{");
  for (int u = 0; u < beacons_count; u++) {
    if (u > 0) Serial.print(",");
    print(beacons_known[u]);
  }
  Serial.print("}");
}
