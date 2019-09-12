#include "VMETrigger.h"

VMETrigger::VMETrigger():Tool(){}


bool VMETrigger::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  reftriggernum=99999;

  TriggerSend=new zmq::socket_t(*(m_data->context),ZMQ_DEALER);
  TriggerSend->bind("tcp://*:6666");


  zmq::socket_t Ireceive (*m_data->context, ZMQ_PUSH);
  Ireceive.connect("inproc://ServicePublish");

  boost::uuids::uuid m_UUID;
  m_UUID = boost::uuids::random_generator()();
  std::stringstream test;
  test<<"Add "<< "TriggerSend "<<m_UUID<<" 6666 "<<"0";
  zmq::message_t send(test.str().length());
  snprintf ((char *) send.data(), test.str().length() , "%s" ,test.str().c_str()) ;

  Ireceive.send(send);
  items[0].socket=*TriggerSend;
  items[0].fd=0;
  items[0].events=ZMQ_POLLIN;
  items[0].revents=0;

  //  m_data->TRG = false; // check this
  m_data->triggernum=0;

  return true;
}


bool VMETrigger::Execute(){

  zmq::poll(&items[0], 1, 0);

  if ((items [0].revents & ZMQ_POLLIN)) {
    zmq::message_t command;
    TriggerSend->recv(&command);

    std::stringstream tmp;
    tmp<<"VME "<<m_data->triggernum;
    zmq::message_t message(tmp.str().length()+1);
    snprintf ((char *) message.data(), tmp.str().length()+1 , "%s" ,tmp.str().c_str()) ;
    TriggerSend->send(message);
    //std::cout<<"VME sending triggernum"<<tmp.str()<<std::endl;
  }


  return true;
}


bool VMETrigger::Finalise(){

  zmq::socket_t Ireceive (*m_data->context, ZMQ_PUSH);
  Ireceive.connect("inproc://ServicePublish");
  std::stringstream test;
  test<<"Delete "<< "TriggerSend ";
  zmq::message_t send(test.str().length()+1);
  snprintf ((char *) send.data(), test.str().length()+1 , "%s" ,test.str().c_str()) ;
  Ireceive.send(send);

  delete TriggerSend;
  TriggerSend=0;


  return true;
}
