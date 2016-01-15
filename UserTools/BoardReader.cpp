#include "BoardReader.h"

BoardReader::BoardReader():Tool(){}


bool BoardReader::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_variables.Get("cards",cards);
  m_variables.Get("channels",channels);
  m_variables.Get("buffersize",buffersize);

 
  m_data->LastSync.resize(cards);
  m_data->SequenceID.resize(cards);
  m_data->StartTime.resize(cards);
  m_data->CardID.resize(cards);
  m_data->channels.resize(cards,channels);
  m_data->buffersize.resize(cards,buffersize);
  m_data->fullbuffsize.resize(cards,channels*buffersize);

  for (int i=0;i<cards;i++){
    m_data->PMTID.push_back(new int[channels]);

  //Data = new double*[channels];
  //for(int i = 0; i < channels; ++i)Data[i] = new double[buffersize];

    m_data->Data.push_back(new double[(channels*buffersize)]);
  }

  /*
  TTree *tree = new TTree("PMTData","PMTData");
 
  tree->Branch("LastSync",&LastSync,"LastSync/I");
  tree->Branch("SequenceID",&SequenceID,"SequenceID/I");
  tree->Branch("StartTime",&StartTime,"StartTime/I");
  tree->Branch("CardID",&CardID,"CardID/I");
  tree->Branch("Channels",&channels,"Channels/I");
  tree->Branch("PMTID",PMTID,"PMTID[Channels]/I");
  tree->Branch("BufferSize",&buffersize,"BufferSize/I");
  tree->Branch("FullBufferSize",&fullbuffsize,"FullBufferSize/I");
  tree->Branch("Data",Data,"Data[FullBufferSize]/D");

  m_data->AddTTree("PMTData",tree);
  */

  return true;
}


bool BoardReader::Execute(){

  for(int card=0;card<cards;card++){
    
    time_t timer;
    struct tm y2k = {0};
    double seconds;
    
    y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
    y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;
    
    time(&timer);  /* get current time; same as: timer = time(NULL)  */
    
        
    m_data->LastSync.at(card)=difftime(timer,mktime(&y2k));
    m_data->SequenceID.at(card)=rand() %1000;
    m_data->StartTime.at(card)=  m_data->LastSync.at(card);
    m_data->CardID.at(card)=card;
    
    
    for(int channel=0;channel<channels;channel++){
      
      m_data->PMTID.at(card)[channel]=(card*4)+channel;

      for(int buffpos=0;buffpos<buffersize;buffpos++){
	
	m_data->Data.at(card)[(channel*buffersize)+buffpos]= (rand() %10000)/10000.0 ;
	
      }
      
    }
    
    
  }
  
  return true;
}


bool BoardReader::Finalise(){

  m_data->LastSync.clear();
  m_data->SequenceID.clear();
  m_data->StartTime.clear();
  m_data->CardID.clear();
  m_data->channels.clear();
  m_data->buffersize.clear();
  m_data->fullbuffsize.clear();

  for (int i=0;i<channels;i++){
    delete m_data->PMTID.at(i);
    m_data->PMTID.at(i)=0;

    delete  m_data->Data.at(i);
    m_data->Data.at(i)=0;
  }
  m_data->PMTID.clear();
  m_data->Data.clear();

  return true;
}
