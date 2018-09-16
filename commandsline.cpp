#include "commandline.h"

// Define the commands 
#define DEAUTH_CLIENT_CMD "deauth_client"
#define FAKE_BEACON_CMD "fake_beacon"
#define PRINT_ALL_CMD "print_all"
#define PRINT_BEACONS_CMD "print_beacons"
#define PRINT_CLIENTS_CMD "print_clients"


/*
 * Function that reads and executes a command from serial
 */
void CommandLine::read() {

  // Check that there is something to read
  if (Serial.available() == 0) {
    return;
  }
  
  char command[64];
  command[Serial.readBytesUntil('\n', command, 63)] = '\0';
  
  char *argument = strchr(command, ' ');
  if (argument != NULL) {
    *argument = '\0';
    argument++;
  }
  
  if (strcmp(command, DEAUTH_CLIENT_CMD) == 0) {
    uint8_t station[ETH_MAC_LEN];
    for (int i = 0; i < ETH_MAC_LEN; i++) {
      station[i] = strtol(argument + 3 * i, NULL, HEX);
    }

    clientinfo *ci = clients->find( station );
    if( ci ) {
      clients->deauth( *ci );
    }

  }
  else if (strcmp(command, FAKE_BEACON_CMD) == 0) {
    char *argument_ssid;
    uint8_t argument_channel = strtol(argument, &argument_ssid, DEC);
    if (argument_ssid != argument) {
      if (*argument_ssid != '\0') argument_ssid++;
      beacons->addFake( argument_channel , argument_ssid );
      
    }
  }
  else if (strcmp(command, PRINT_ALL_CMD) == 0) {
    Serial.print("{\"beacons\":");
    beacons->print();
    Serial.print(",\"clients\":");
    clients->print();
    Serial.print("}");
  }
  else if (strcmp(command, PRINT_BEACONS_CMD) == 0) {
    beacons->print();
  }
  else if (strcmp(command, PRINT_CLIENTS_CMD) == 0) {
    clients->print();
  }
  Serial.println("\n");
}
