#ifndef BEACONS
#define BEACONS

#include "structures.h"

class Beacons {
  public:
    Beacons();
   
    struct beaconinfo parse(uint8_t *frame, uint16_t framelen, signed rssi);
    
    boolean store(beaconinfo bi);
    
    void print(beaconinfo bi);
    void print();

  private:
    unsigned int beacons_count = 0;
    unsigned int beacons_index = 0;
    beaconinfo beacons_known[MAX_BEACONS];
   
};


#endif
