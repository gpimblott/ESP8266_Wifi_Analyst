#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "Arduino.h"

/*
 * Constants.
 */
#define ETH_MAC_LEN 6
#define MAX_BEACONS 256
#define MAX_CLIENTS 256
#define MAX_SSID_LEN 40

/**
 *  Structures used in the program
 */

// Promiscous callback structures, see ESP manual
struct RxControl {
  signed rssi: 8;
  unsigned rate: 4;
  unsigned is_group: 1;
  unsigned: 1;
  unsigned sig_mode: 2;
  unsigned legacy_length: 12;
  unsigned damatch0: 1;
  unsigned damatch1: 1;
  unsigned bmatch0: 1;
  unsigned bmatch1: 1;
  unsigned MCS: 7;
  unsigned CWB: 1;
  unsigned HT_length: 16;
  unsigned Smoothing: 1;
  unsigned Not_Sounding: 1;
  unsigned: 1;
  unsigned Aggregation: 1;
  unsigned STBC: 2;
  unsigned FEC_CODING: 1;
  unsigned SGI: 1;
  unsigned rxend_state: 8;
  unsigned ampdu_cnt: 8;
  unsigned channel: 4;
  unsigned: 12;
};

struct sniffer_buf1 {
  struct RxControl rx_ctrl;
  uint8_t buf[112];
  uint16_t cnt;
  uint16_t len;
};

struct sniffer_buf2 {
  struct RxControl rx_ctrl;
  uint8_t buf[36];
  uint16_t cnt;
  struct {
    uint16_t len;
    uint16_t seq;
    uint8_t  address3[ETH_MAC_LEN];
  } lenseq[1];
};


/*
 *  Structure for beacon information
 */
struct beaconinfo {
  uint8_t beacon[ETH_MAC_LEN];
  uint8_t ssid[33];
  uint8_t ssid_len;
  uint8_t channel;
  uint8_t rssi;
  bool err;
};

/*
 *  Structure for client information
 */
struct clientinfo {
  uint8_t beacon[ETH_MAC_LEN];
  uint8_t station[ETH_MAC_LEN];
  uint8_t rssi;
  uint16_t seq;
  bool err;
};
#endif
