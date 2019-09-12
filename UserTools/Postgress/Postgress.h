#ifndef Postgress_H
#define Postgress_H

#include <string>
#include <iostream>
#include <sstream>
#include <pqxx/pqxx>
#include <sys/time.h>

#include "Tool.h"

class Postgress: public Tool {


 public:

  Postgress();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  bool m_verbose;
  void PSQLSync();
  std::vector<pqxx::connection*> C;
  



};


#endif
