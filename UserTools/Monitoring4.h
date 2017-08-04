#ifndef Monitoring_H
#define Monitoring_H

#include <string>
#include <iostream>
#include <sstream>

#include "Tool.h"

#include "TH1I.h"
#include "TH2I.h"
#include "TCanvas.h"
#include "TThread.h"
#include "TLegend.h"
#include "TGraph2D.h"
#include "TF1.h"
#include "Plot.h"
#include "OriginalPlots.h"

#include <pqxx/pqxx>

struct monitor_thread_args{


  monitor_thread_args( zmq::context_t* incontext, Store* invariables){
    context=incontext;
    variables=invariables;
  }
  
  zmq::context_t* context;
  Store* variables;
};
/*
struct PMT{

  int gx,gy,gz,card,channel;
  float PEDmean,PEDsigma;

};
*/
class Monitoring: public Tool {


 public:

  Monitoring();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  zmq::socket_t *Isend;
  static void *MonitorThread(void* arg);
  //  pthread_t thread;
  TThread *h1;
  monitor_thread_args *args;
  std::vector<int> MonitoringLevels;

};


#endif
