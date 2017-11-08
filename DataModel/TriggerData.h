#ifndef TRIGGERDATA_H
#define TRIGGERDATA_H

#include <stdint.h>
#include <iostream>
#include <vector>

#include "zmq.hpp"

//#include <boost/iostreams/stream.hpp>
//#include <boost/iostreams/device/back_inserter.hpp>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/serialization/vector.hpp>
#include "timestamp.h"
#include "eventlog.h"
#include "TTree.h"

struct TriggerData{
//  friend class boost::serialization::access;
  TriggerData();
  void Init();
  TTree *tree;
  void Fill(){
    //    std::cout<<"eventsize "<<EventSize<<std::endl;
    //std::cout<<"triggersize "<<TriggerSize<<std::endl;
tree->Fill();}
  void Write(){tree->Write();}
  int FirmwareVersion;
  int SequenceID;
  int EventSize;
  //  std::vector<uint16_t> EventIDs;
  //std::vector<uint64_t> EventTimes;  // units of nanoseconds.
  unsigned short EventIDs[500];
    unsigned long EventTimes[500];
  int TriggerSize;
  // std::vector<uint32_t> TriggerMasks;

  //std::vector<uint32_t> TriggerCounters;
   unsigned  int TriggerMasks[50000];
   uint32_t TriggerCounters[50000];  
  int FIFOOverflow;
  int DriverOverflow;
  

  void  Send(zmq::socket_t *socket);
  bool Receive(zmq::socket_t *socket);

//  template<class Archive>
//  void serialize(Archive & ar, const unsigned int version) {
//    ar & FirmwareVersion;
//    ar & SequenceID;
//    ar & eventsize;
//    ar & Events;
//    ar & datasize;
//    ar & Data;
//    ar & FIFOOverflow;
//    ar & DriverOverflow;
//  }
//  
};

//template <class T>
//std::string serialize_save(const T& obj) {
//  std::string outstr;
//  boost::iostreams::back_insert_device<std::string> inserter(outstr);
//  boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
//  boost::archive::text_oarchive oa(s);
//  oa << obj;
//  s.flush();
//  return outstr;
//}
//
//template <class T>
//void deserialize_load(const std::string data, size_t size) {
//  T obj;
//  boost::iostreams::basic_array_source<char> device(data.data(), size);
//  boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
//  boost::archive::text_iarchive ia(s);
//  ia >> obj;
//}

#endif
