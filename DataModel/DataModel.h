#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <map>
#include <string>
#include <vector>

#include "TTree.h"

#include "Store.h"

#include <zmq.hpp>

class DataModel{


 public:
  
  DataModel();
  zmq::context_t* context;

  TTree* GetTTree(std::string name);
  void AddTTree(std::string name,TTree *tree);
  void DeleteTTree(std::string name);
  
  Store vars;

  //Run info
  long RunNumber;
  long SubRunNumber;
  int RunType;
  long NumEvents;


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



 private:
  
  std::map<std::string,TTree*> m_trees; 
  
  
};



#endif
