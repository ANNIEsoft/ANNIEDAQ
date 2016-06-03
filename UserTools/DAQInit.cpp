#include "DAQInit.h"

DAQInit::DAQInit():Tool(){}


bool DAQInit::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_variables.Get("result", m_data->Result);

  return true;
}


bool DAQInit::Execute(){

  return true;
}


bool DAQInit::Finalise(){

  return true;
}
