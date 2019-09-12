#include "Trigger.h"

Trigger::Trigger():Tool(){}


bool Trigger::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_variables.Get("Trigger_port",Trigger_port);
  m_variables.Get("verbose",m_verbose);

  m_data= &data;

  sock=new zmq::socket_t(*(m_data->context), ZMQ_REP);

  std::stringstream tmp;
  tmp<<"tcp://*:"<<Trigger_port;
  sock->bind(tmp.str().c_str());

  in[0].socket=sock;
  in[0].events=ZMQ_POLLIN;
  out[0].socket=sock;
  out[0].events=ZMQ_POLLOUT; 

  m_data->enabled=false;

  return true;
}


bool Trigger::Execute(){


  if(m_data->enabled) zmq_poll(&in[0], 1, 0);
  else zmq_poll(&in[0], 1, 100);

  if (in[0].revents & ZMQ_POLLIN) {
    zmq::message_t msg;
    sock->recv(&msg);
    std::istringstream iss(static_cast<char*>(msg.data()));
    
    if(iss.str()=="Enable") m_data->enabled=true;
    else if(iss.str()=="Disable") m_data->enabled=false; 
    
    zmq_poll(&out[0], 1, 100);

    if (out[0].revents & ZMQ_POLLOUT) {
      
      zmq::message_t rep(4);
      printf ((char *) rep.data(), 4 , "%s" ,"AKN" );
      sock->send(rep);
    }
    
  }
  


  return true;
}


bool Trigger::Finalise(){

  m_data->enabled=false;
  delete sock;
  sock=0;

  return true;
}
