#include "VMESendData.h"

VMESendData::VMESendData():Tool(){}


bool VMESendData::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_variables.Get("port",m_port);
  
  Send=new zmq::socket_t(*(m_data->context), ZMQ_PUSH);
  //  int a=12000;
  //Send->setsockopt(ZMQ_SNDTIMEO, a);
  //Send->setsockopt(ZMQ_RCVTIMEO, a);

  std::stringstream tmp;
  tmp<<"tcp://*:"<<m_port;
  
  Send->bind(tmp.str().c_str());
  
  return true;
}


bool VMESendData::Execute(){
 
  if(m_data->triggered){
    //std::cout<<"d 1 "<<std::endl; 
    // zmq::message_t Esend(512);
    //snprintf ((char *) Esend.data(), 256 , "%s" ,command.c_str()) ;
    //  StatusCheck.send(Esend);
    
    // create and open a character archive for output
    //std::ofstream ofs("testser");
    std::ostringstream archive_stream;
    // save data to archive
    //std::cout<<"d 2 "<<std::endl;
    boost::archive::text_oarchive oa(archive_stream);
    //std::cout<<"d 3 "<<m_data->carddata.size()<<std::endl;  
  // write class instance to archive
    for (int i=0;i<m_data->carddata.size();i++){
      //std::cout<<"d 4 "<<std::endl;
      oa << *(m_data->carddata.at(i));
    }
    // archive and stream closed when destructors are called
     
    //std::cout<<"d 5 "<<std::endl;
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
    //std::cout<<"d 6 "<<std::endl;
    std::string tmp=archive_stream.str();
    //std::cout<<tmp<<std::endl;
    //std::cout<<"d 7 "<<std::endl;
    zmq::message_t message(tmp.length()+1);
    snprintf ((char *) message.data(),  tmp.length()+1 , "%s" ,tmp.c_str()) ;
    //std::cout<<"d 8 "<<std::endl;
    Send->send(message);
    //std::cout<<"d 9 "<<std::endl;
    m_data->Crate->EnableTrigger();
  }

  return true;
}


bool VMESendData::Finalise(){
  
  delete  Send;
  Send=0;

  return true;
}
