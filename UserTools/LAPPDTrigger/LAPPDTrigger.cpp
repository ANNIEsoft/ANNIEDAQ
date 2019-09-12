#include "LAPPDTrigger.h"

LAPPDTrigger::LAPPDTrigger():Tool(){}


bool LAPPDTrigger::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  return true;
}


bool LAPPDTrigger::Execute(){

  return true;
}


bool LAPPDTrigger::Finalise(){

  return true;
}
