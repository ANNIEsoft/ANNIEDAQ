#ifndef StoreSave_H
#define StoreSave_H

#include <string>
#include <iostream>

#include "Tool.h"

class StoreSave: public Tool {


 public:

  StoreSave();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:
  
  std::map<std::string,zmq::socket_t*> DataSources;
  BoostStore* outstore;
  std::string OutFile;
  std::string OutPath;
  std::string Out;
  int m_verbose;
  int numSources;
  int part;

  void FindDataSources();

};


#endif
