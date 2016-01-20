#ifndef Monitoring_H
#define Monitoring_H

#include <string>
#include <iostream>
#include <sstream>

#include "Tool.h"


struct monitor_thread_args{


  monitor_thread_args(int monitorport, zmq::context_t* incontext){
    MonitorPort=monitorport;
    context=incontext;
  }
  
  zmq::context_t* context;
  int MoniterPort;
  
};



class Monitoring: public Tool {


 public:

  Monitoring();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  int MonitoringLevel;
  int MonitorPort;

  zmq::socket_t *Isend;
  static void *MonitorThread(void* arg);
  pthread_t thread;

  std::vector<int> LastSync;
  std::vector<int> SequenceID;
  std::vector<int> StartTime;
  std::vector<int> CardID;
  std::vector<int> channels;
  std::vector<int*> PMTID;
  std::vector<int> buffersize;
  std::vector<int> fullbuffsize;
  std::vector<double*> Data;



};


#endif
