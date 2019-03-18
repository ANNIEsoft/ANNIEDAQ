#include "MRDMonitoring.h"

MRDMonitoring::MRDMonitoring():Tool(){}


bool MRDMonitoring::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_variables.Get("MonitoringPeriod",period);

  cleanup=false;
  //  num=0;

  m_data->MonitoringSocket=0;

  if(m_data->MonitoringSocket==0){

    cleanup=true;
    m_data->MonitoringSocket=new zmq::socket_t(*m_data->context, ZMQ_PUB);
    m_data->MonitoringSocket->bind("tcp://*:88888");

    zmq::socket_t Ireceive (*m_data->context, ZMQ_PUSH);
    Ireceive.connect("inproc://ServicePublish");

    boost::uuids::uuid m_UUID;
    m_UUID = boost::uuids::random_generator()();
    std::stringstream test;
    test<<"Add "<< "MonitorData "<<m_UUID<<" 88888 "<<"0";
    zmq::message_t send(test.str().length());
    snprintf ((char *) send.data(), test.str().length() , "%s" ,test.str().c_str()) ;
    Ireceive.send(send);  

}

  return true;
}


bool MRDMonitoring::Execute(){

  //  if (m_data->MRDdata.TRG){

  //    num++;
  // }

  if( m_data->MRDdata.triggernum % period==0){
  zmq::message_t message(10);                  
  snprintf ((char *) message.data(), 10 , "%s" ,"MRDSingle") ;
  m_data->MonitoringSocket->send(message, ZMQ_SNDMORE);

  m_data->MRDout.Send(m_data->MonitoringSocket);
  }

  return true;
}


bool MRDMonitoring::Finalise(){

  if(cleanup){

    zmq::socket_t Ireceive (*m_data->context, ZMQ_PUSH);
    Ireceive.connect("inproc://ServicePublish");
    std::stringstream test;
    test<<"Delete "<< "MonitorData ";
    zmq::message_t send(test.str().length()+1);
    snprintf ((char *) send.data(), test.str().length()+1 , "%s" ,test.str().c_str()) ;
    Ireceive.send(send);


    delete m_data->MonitoringSocket;
    m_data->MonitoringSocket==0;
  }

  return true;
}
