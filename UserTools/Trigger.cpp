#include "Trigger.h"

Trigger::Trigger():Tool(){}


bool Trigger::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  return true;
}


bool Trigger::Execute(){
 
  usleep(50000);
 

  /*  while (true){
    if ((rand() %100)>(rand() %100)) break;
  }
  */
  return true;
}


bool Trigger::Finalise(){

  return true;
}
