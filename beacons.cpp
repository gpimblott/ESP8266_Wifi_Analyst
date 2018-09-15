#include "beacons.h"

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
