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
  
  //  std::cout<<" debug 0 "<<m_data->Crate->NumberOfCards()<<std::endl;
  // m_data->triggered=m_data->Crate->Triggered();

  //  std::cout<<" debug 1 "<<std::endl;

  //m_data->carddata.clear();
  
  //std::cout<<" debug 2 "<<std::endl;

  if (m_data->triggered){
    
    //   for (int i=0;i< m_data->carddata.size();i++){
    //  delete m_data->carddata.at(i);
    //  }
    m_data->carddata.clear();
    delete m_data->triggerdata;
    m_data->triggerdata=0;   
    //std::cout<<" debug 3 "<<std::endl;
    
    //CardData *data=new CardData();    
    //std::cout<<" debug 4 "<<m_data->Crate->NumberOfCards()<<std::endl;
    // for(int i=0 ;i<m_data->Crate->NumberOfCards();i++){
    // m_data->carddata.push_back(m_data->Crate->GetCardData(i)); //normally loop over all cards
      //std::cout<<" debug 5 "<<std::endl;
      //m_data->carddata.push_back(data);
      //std::cout<<" debug 6 "<<std::endl;
    //}
    // m_data->Crate->EnableTrigger();
    m_data->carddata=  m_data->Crate->GetCardData();

    ///////////////////// BEN faking card data remove ////////
    for( int i=0;i<10;i++){
      CardData tmp;
      tmp.CardID=i;
      tmp.SequenceID=i;
      tmp.FirmwareVersion=5;
      for( int j=0;j<5;j++){
	tmp.Data.push_back(j+i);
	}
      //std::cout<< "size="<<tmp.Data.size()<<std::endl;
      m_data->carddata.push_back(tmp);
    
}
    
    ///////////////////////////////////////////
    
    if(m_data->CrateNum==1){
      //  std::cout<<"getting trig data"<<std::endl;


	if(m_data->TriggerCard->HasData()){
	  //  std::cout<<"in has trig data"<<std::endl;
	  m_data->triggerdata=m_data->TriggerCard->GetTriggerData();
	}
      ////////// BEN faking trigger data remove ///////
	m_data->triggerdata = new TriggerData();
	//std::cout<<"w1"<<std::endl;

	m_data->triggerdata->FirmwareVersion=5;
	m_data->triggerdata->SequenceID=4;
	m_data->triggerdata->EventSize=10;
	m_data->triggerdata->TriggerSize=10;
	m_data->triggerdata->FIFOOverflow=0;
	m_data->triggerdata->DriverOverflow=0;
	//std::cout<<"w2"<<std::endl;

	for( int i=0;i<10;i++){
	  //std::cout<<"w3 "<<i<<std::endl;
	  m_data->triggerdata->EventIDs.push_back((uint16_t)i);
	  m_data->triggerdata->EventTimes.push_back((uint64_t)i);
	  m_data->triggerdata->TriggerMasks.push_back((uint32_t)i);
	  m_data->triggerdata->TriggerCounters.push_back((uint32_t)i);
	}
	std::cout<<"w3.5 "<< m_data->triggerdata->EventSize<<std::endl;
	std::cout<<"w4 "<< m_data->triggerdata->EventIDs.size()<<std::endl;
	std::cout<<"wr "<< m_data->triggerdata->EventIDs.at(3)<<std::endl;

	///////////////////////////////
	
	
    }
    
  }
  
  
  
  //std::cout<<" debug 7 "<<std::endl;
  return true;
}


bool CrateReader::Finalise(){
  
  //for (int i=0;i< m_data->carddata.size();i++){
  //  delete m_data->carddata.at(i);
  //} 
  //std::cout<<"d1"<<std::endl;
  m_data->carddata.clear();
 //std::cout<<"d2"<<std::endl;
  delete m_data->triggerdata;
 //std::cout<<"d3"<<std::endl;
  m_data->triggerdata=0;
 //std::cout<<"d4"<<std::endl;
 // m_data->TriggerCard->Finalise();  
 //std::cout<<"d4.1"<<std::endl;  
 // m_data->Crate->Finalise();
  //std::cout<<"d4.2"<<std::endl; 
 //std::cout<<"d5"<<std::endl;
  delete m_data->Crate;
 //std::cout<<"d6"<<std::endl;
  m_data->Crate=0;
 //std::cout<<"d7"<<std::endl;

  return true;
}
