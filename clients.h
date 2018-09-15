#ifndef CLIENT_H
#define CLIENT_H

#include "structures.h"

class Clients {
  public:
    Clients();
    struct clientinfo parse(uint8_t *frame, uint16_t framelen, signed rssi);
    
    boolean store(clientinfo ci);

    clientinfo *find(uint8_t *station);
    
    void print(clientinfo ci);
    void print();
    
  private:
    unsigned int clients_count = 0;
    unsigned int clients_index = 0;

    clientinfo clients_known[MAX_CLIENTS];

    uint8_t broadcast1[3] = { 0x01, 0x00, 0x5e };
    uint8_t broadcast2[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    uint8_t broadcast3[3] = { 0x33, 0x33, 0x00 };
};


#endif
