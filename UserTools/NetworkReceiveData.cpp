#include "NetworkReceiveData.h"

NetworkReceiveData::NetworkReceiveData():Tool(){}


bool NetworkReceiveData::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_variables.Get("port",m_port);
  m_variables.Get("address",m_address);


  Receive=new zmq::socket_t(*(m_data->context), ZMQ_PULL);
  std::stringstream tmp;
  tmp<<"tcp://localhost:"<<"24011";
  Receive->connect(tmp.str().c_str());

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



  return true;
}


bool NetworkReceiveData::Execute(){


  if(m_data->triggered){
    
    zmq::message_t message;
    Receive->recv(&message);
    std::istringstream iss(static_cast<char*>(message.data()));
    
    //  std::cout<<iss.str()<<std::endl;;
    //  std::ostringstream archive_stream;
    
    boost::archive::text_iarchive ia(iss);
    
    //  std::cout<<"run number before is "<<m_data->RunNumber<<std::endl;
    ia >> *m_data;
    // std::cout<<"run number after is "<<m_data->RunNumber<<std::endl;
    //std::cout<<"received data 3 "<<*(m_data->Data.at(3))<<std::endl;
    
  }
  
  return true;
}


bool NetworkReceiveData::Finalise(){


  delete Receive;
  Receive=0;

  for (int i=0;i<m_data->PMTID.size();i++){

    delete m_data->PMTID.at(i);

  }

  m_data->PMTID.clear();


  for (int i=0;i< m_data->Data.size();i++){

    delete m_data->Data.at(i);

  }

  m_data->Data.clear();

  return true;
}
