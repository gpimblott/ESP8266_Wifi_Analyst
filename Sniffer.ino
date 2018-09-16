/*
   By Gordon Pimblott Sept 2018 on Arduino 1.8.6

   Distributed under the MIT license (URL)

   Based on lots of other great projects
    * Lars Juhl Jensen 20170415 compiled on OS X using Arduino 1.8.2
    * Ray Burnette's ESP8266 Mini Sniff (MIT) https://www.hackster.io/rayburne/esp8266-mini-sniff-f6b93a
    * RandDruid/esp8266-deauth (MIT) https://github.com/RandDruid/esp8266-deauth
*/

#include <ESP8266WiFi.h>

#include "esp_functions.h"
#include "commandline.h"
#include "structures.h"
#include "beacons.h"
#include "clients.h"


uint8_t channel = 1;
uint8_t nothing_new = 0;

Beacons beacons;
Clients clients;

CommandLine commandLine(&beacons , &clients);




/*
 *  Callback function for promiscuous mode that parses received packet
 */
void parse_packet(uint8_t *buf, uint16_t len) {
  int i = 0;
  if (len == 12) {
    struct RxControl *sniffer = (struct RxControl*) buf;
  } else if (len == 128) {
    struct sniffer_buf1 *sniffer = (struct sniffer_buf1*) buf;
    struct beaconinfo bi = beacons.parse(sniffer->buf, 112, sniffer->rx_ctrl.rssi);
    if (bi.err == 0 && beacons.store(bi) == 0) nothing_new = 0;
  } else {
    struct sniffer_buf2 *sniffer = (struct sniffer_buf2*) buf;
    if ((sniffer->buf[0] == 0x08) || (sniffer->buf[0] == 0x88)) {
      struct clientinfo ci = clients.parse(sniffer->buf, 36, sniffer->rx_ctrl.rssi);
      if (memcmp(ci.beacon, ci.station, ETH_MAC_LEN)) {
        if (ci.err == 0 && clients.store(ci) == 0) nothing_new = 0;
      }
    }
  }
}


/*
 *  Initial setup
 */
void setup() {
  Serial.begin(115200);
  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(channel);
  wifi_promiscuous_enable(0);
  wifi_set_promiscuous_rx_cb(parse_packet);
  wifi_promiscuous_enable(1);

  Serial.printf("Starting Sniffer\n");
  Serial.printf("Flash size : %d\n", ESP.getFlashChipSize()/1024);
}

/*
 * Main loop
 */
void loop() {
  if (nothing_new >= 10) {
    nothing_new = 0;
    channel++;
    if (channel == 15) channel = 1;
    wifi_set_channel(channel);
  }
  else {
    nothing_new++;
  }

  // Send any fake beacons that have been setup
  beacons.sendFake(channel , 3);
  
  delay(1);
  
  // Read and process any
  commandLine.read();

}
