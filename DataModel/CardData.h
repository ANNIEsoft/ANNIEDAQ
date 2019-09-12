#ifndef CARDDATA_H
#define CARDDATA_H

#include <stdint.h>
#include <iostream>
#include <vector>
#include <array>
#include <stdlib.h>
#include "zmq.hpp"

struct CardData{
  int CardID;
  int SequenceID;
  int FirmwareVersion;
  std::vector<uint32_t> Data;
  
  ~CardData();

  void  Send(zmq::socket_t *socket, int flag=0);
  void Receive(zmq::socket_t *socket);

  
};


#endif
