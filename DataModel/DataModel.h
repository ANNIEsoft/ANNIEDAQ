#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <map>
#include <string>
#include <vector>

#include "TTree.h"

#include "Store.h"
#include "BoostStore.h"
#include "Logging.h"
#include "CardData.h"
#include "MRDData.h"
#include "TriggerData.h"
#include "MRDOut.h"

#include <zmq.hpp>

class DataModel{


 public:
  
  DataModel();
  zmq::context_t* context;

  TTree* GetTTree(std::string name);
  void AddTTree(std::string name,TTree *tree);
  void DeleteTTree(std::string name);
  
  Store vars;
  BoostStore CStore;
  std::map<std::string,BoostStore*> Stores;
  Logging *Log;

  //Run info
  long RunNumber;
  long SubRunNumber;
  int RunType;
  long NumEvents;
  bool triggered;
  int Restart;
  long triggernum;
  //Board Data
  std::vector<int> LastSync;
  std::vector<int> SequenceID;
  std::vector<int> StartTime;
  std::vector<int> CardID;
  std::vector<int> channels;
  std::vector<int*> PMTID;
  std::vector<int> buffersize;
  std::vector<int> fullbuffsize;
  std::vector<double*> Data;

  std::vector<CardData*> carddata; 
  TriggerData *trigdata;
  
  std::string InfoTitle;
  std::string InfoMessage;

  MRDData MRDdata;
  MRDOut MRDout;

  zmq::socket_t *MonitoringSocket;

 private:
  
  std::map<std::string,TTree*> m_trees; 
  
  
};



#endif
