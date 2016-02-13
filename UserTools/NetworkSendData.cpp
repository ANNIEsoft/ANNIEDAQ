#include "NetworkSendData.h"

NetworkSendData::NetworkSendData():Tool(){}


bool NetworkSendData::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_variables.Get("port",m_port);
  
  Send=new zmq::socket_t(*(m_data->context), ZMQ_PUSH);
  std::stringstream tmp;
  tmp<<"tcp://*:"<<m_port;
  Send->bind(tmp.str().c_str());
  
  return true;
}


bool NetworkSendData::Execute(){
  
  if(m_data->triggered){
    
    // zmq::message_t Esend(512);
    //snprintf ((char *) Esend.data(), 256 , "%s" ,command.c_str()) ;
    //  StatusCheck.send(Esend);
    
    // create and open a character archive for output
    //std::ofstream ofs("testser");
    std::ostringstream archive_stream;
    // save data to archive
    
    boost::archive::text_oarchive oa(archive_stream);
    // write class instance to archive
    oa << *m_data;
    // archive and stream closed when destructors are called
    
    
    //std::cout<<"run number = "<<m_data->RunNumber<<std::endl;
    // std::cout<<"sub run number = "<<m_data->SubRunNumber<<std::endl;
    //std::cout<<"data 3 = "<<*(m_data->Data.at(3))<<std::endl;
    // ... some time later restore the class instance to its orginal state
    /* gps_position newg;
       
       {
       // create and open an archive for input
       std::ifstream ifs("filename");
       boost::archive::text_iarchive ia(ifs);
       // read class state from archive
       ia >> newg;
       // archive and stream closed when destructors are called
       }
    */
    std::string tmp=archive_stream.str();
    //std::cout<<tmp<<std::endl;
    
    zmq::message_t message(tmp.length()+1);
    snprintf ((char *) message.data(),  tmp.length()+1 , "%s" ,tmp.c_str()) ;
    Send->send(message);

    
  }
  return true;
}


bool NetworkSendData::Finalise(){
  
  delete  Send;
  Send=0;

  return true;
}
