#ifndef BEACONS
#define BEACONS

#include "structures.h"
#include "esp_functions.h"




class Beacons {
  public:
    Beacons();
   
    struct beaconinfo parse(uint8_t *frame, uint16_t framelen, signed rssi);
    boolean store(beaconinfo bi);
    
    void sendFake(byte channel, uint8_t packets); 
    void addFake(int channel, char *ssid);
    
    void print(beaconinfo bi);
    void print();

  private:
    unsigned int beacons_count = 0;
    unsigned int beacons_index = 0;
    beaconinfo beacons_known[MAX_BEACONS];
    
    char fake_beacon_ssid[14][MAX_SSID_LEN];   
};


#endif
