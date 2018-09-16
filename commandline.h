#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include "structures.h"
#include "beacons.h"
#include "clients.h"

class CommandLine {
  
  public:
    CommandLine(Beacons *_beacons, Clients *_clients) : beacons(_beacons), clients(_clients){};
    
    void read();

  private:
    Beacons *beacons;
    Clients *clients;
  
};

#endif
