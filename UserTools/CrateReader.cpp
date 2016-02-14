#include "CrateReader.h"

CrateReader::CrateReader():Tool(){}


bool CrateReader::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;
  
  //m_variables.Get("Crate_Num",m_crate_num);
  
  //std::time_t begin, end;
  //CardData data;
  
  // Crate= new UC500ADCInterface(m_crate_num);
  // Crate->Initialise();
  
  //  m_num_cards=Crate->NumberOfCards();

  //  Crate->EnableTrigger();

  return true;

}


bool CrateReader::Execute(){
  
  
  

  //std::cout<<" debug 1 "<<std::endl;

  //m_data->carddata.clear();
  
  // std::cout<<" debug 2 "<<std::endl;

  if (m_data->triggered){
    
    for (int i=0;i< m_data->carddata.size();i++){
      delete m_data->carddata.at(i);
    }
    m_data->carddata.clear();
    
    //std::cout<<" debug 3 "<<std::endl;
    
    //CardData *data=new CardData();    
    //std::cout<<" debug 4 "<<std::endl;
    m_data->carddata.push_back(m_data->Crate->GetCardData(5)); //normally loop over all cards
    //std::cout<<" debug 5 "<<std::endl;
    //m_data->carddata.push_back(data);
    //std::cout<<" debug 6 "<<std::endl;
  }

  //std::cout<<" debug 7 "<<std::endl;
  return true;
}


bool CrateReader::Finalise(){
  
  for (int i=0;i< m_data->carddata.size();i++){
    delete m_data->carddata.at(i);
  } 
  m_data->carddata.clear();
  
  m_data->Crate->Finalise();
  delete m_data->Crate;
  m_data->Crate=0;

  return true;
}
