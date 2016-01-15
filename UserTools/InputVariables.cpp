#include "InputVariables.h"

InputVariables::InputVariables():Tool(){}


bool InputVariables::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_variables.Get("RunType",m_data->RunType);

  return true;
}


bool InputVariables::Execute(){

  return true;
}


bool InputVariables::Finalise(){

  return true;
}
