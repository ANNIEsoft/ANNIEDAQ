#ifndef Postgress_H
#define Postgress_H

#include <string>
#include <iostream>
#include <sstream>
#include <pqxx/pqxx>

#include "Tool.h"

class Postgress: public Tool {


 public:

  Postgress();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  bool m_verbose;

  pqxx::connection *C;
  



};


#endif
