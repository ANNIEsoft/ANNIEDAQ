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
  m_data->trigdata=0;

  return true;

}


bool CrateReader::Execute(){
  
  //  std::cout<<" debug 0 "<<m_data->Crate->NumberOfCards()<<std::endl;
  // m_data->triggered=m_data->Crate->Triggered();

  //  std::cout<<" debug 1 "<<std::endl;

  //m_data->carddata.clear();
  
  //std::cout<<" debug 2 "<<std::endl;

  m_data->Trigger->HasData();

  if (m_data->triggered){

    m_data->Trigger->DisableADCTriggers();
    
    for (int i=0;i< m_data->carddata.size();i++){
      delete m_data->carddata.at(i);
    }
    m_data->carddata.clear();


        if(m_data->trigdata!=0){
      delete m_data->trigdata;
      m_data->trigdata=0;
    }
    
    //std::cout<<" debug 3 "<<std::endl;
    
    //CardData *data=new CardData();    
    //std::cout<<" debug 4 "<<m_data->Crate->NumberOfCards()<<std::endl;
    for(int i=0 ;i<m_data->Crate->NumberOfCards();i++){
      m_data->carddata.push_back(m_data->Crate->GetCardData(i)); //normally loop over all cards
      //std::cout<<" debug 5 "<<std::endl;
      //m_data->carddata.push_back(data);
      //std::cout<<" debug 6 "<<std::endl;
    }
    m_data->trigdata=m_data->Trigger->GetTriggerData();
    m_data->Trigger->EnableTrigger();
  }
  
  //std::cout<<" debug 7 "<<std::endl;
  return true;
}


bool CrateReader::Finalise(){
  
  for (int i=0;i< m_data->carddata.size();i++){
    delete m_data->carddata.at(i);
  } 
  m_data->carddata.clear();
  
  if(m_data->trigdata!=0){
    delete m_data->trigdata;
    m_data->trigdata=0;
  }

  m_data->Crate->Finalise();
  delete m_data->Crate;
  m_data->Crate=0;

  m_data->Trigger->Finalise();
  delete m_data->Trigger;
  m_data->Trigger=0;
  
  return true;
}
