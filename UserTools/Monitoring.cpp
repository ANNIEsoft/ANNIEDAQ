#include "Monitoring.h"

Monitoring::Monitoring():Tool(){}


bool Monitoring::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  return true;
}


bool Monitoring::Execute(){

  return true;
}


bool Monitoring::Finalise(){

  return true;
}
