#ifndef CARDDATA_H
#define CARDDATA_H

#include <stdint.h>
#include <iostream>
#include <stdlib.h>

#include "zmq.hpp"


struct CardData{

  int LastSync;
  int SequenceID;
  int StartTime;
  int CardID;
  int channels; // eg number of connected PMTs
  int *PMTID; //1D array of length channels
  int buffersize;
  int fullbuffsize; // buffersize * num channels
  uint16_t *Data; //1D array of card readout size fullbuffsize
  
  //the form and structure of this data is probably something you want to change this was just my guess baised on what we talked about.
  //so feel free to make this what ever you making the card output to be
  
  ~CardData();  

  void  Send(zmq::socket_t *socket);
  void Receive(zmq::socket_t *socket);
  
 
  
};


#endif
