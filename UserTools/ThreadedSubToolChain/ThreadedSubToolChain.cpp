#include "ToolChain.h"
#include "ThreadedSubToolChain.h"

ThreadedSubToolChain::ThreadedSubToolChain():Tool(){}


bool ThreadedSubToolChain::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();
  std::cout<<"d1"<<std::endl;
  m_data= &data;

  std::cout<<"d2"<<std::endl;

  args = new ToolChain_thread_args(configfile);
  std::cout<<"d3"<<std::endl;

  pthread_create (&thread, NULL, ThreadedSubToolChain::SubThread, args);
  //h1 = new TThread("h1", ThreadedSubToolChain::SubThread, args);
  //h1->Run();
  printf("d4");
  
  std::cout<<"d4"<<std::endl;
  printf("d4.5");
  
  //  sleep(1);
  printf("d5");
  std::cout<<"d5"<<std::endl;

  //Isend= new zmq::socket_t(*(m_data->context),ZMQ_REQ);
  zmq::socket_t Isend(*(m_data->context),ZMQ_REQ);  
  std::stringstream connection;
  int port;
  m_variables.Get("remote_port",port);
  connection<<"tcp://127.0.0.1:"<<port;
  Isend.connect(connection.str());
  printf("d6");
  std::cout<<"d6"<<std::endl;

  zmq::pollitem_t items [] = {
    { Isend, 0, ZMQ_POLLOUT, 0 },
  };

  Store rr;
 
  rr.Set("msg_type","Command");
  rr.Set("msg_value","Start");

  

  std::string tmp="";
  rr>>tmp;
  zmq::message_t send(tmp.length()+1);  
  snprintf ((char *) send.data(), tmp.length()+1 , "%s" ,tmp.c_str()) ;
  printf("d7");
  std::cout<<"d7"<<std::endl;

  zmq::poll(&items [0], 1, -1);

  if ((items [0].revents & ZMQ_POLLOUT)) {

  Isend.send(send);
  printf("d8");
  std::cout<<"d8"<<std::endl;


  zmq::message_t message;
  printf("d9"); 
 std::cout<<"d9"<<std::endl;
  Isend.recv (&message);
  printf("d10");  
std::cout<<"d10"<<std::endl;
  std::istringstream iss(static_cast<char*>(message.data()));
  std::cout<<iss.str()<<std::endl;

  //delete Isend;
  
  //m_data->InfoTitle="MRDVariables";
  // m_variables>>m_data->InfoMessage;
  //m_data->GetTTree("RunInformation")->Fill();
  }

  //sleep(5);
  return true;
}


bool ThreadedSubToolChain::Execute(){

  //  sleep(5);
  if(m_data->Restart==1)Finalise();
  else if(m_data->Restart==2)Initialise("",*m_data);

  return true;
}


bool ThreadedSubToolChain::Finalise(){



  std::cout<<"s1"<<std::endl;
  
  //Isend= new zmq::socket_t(*(m_data->context),ZMQ_REQ);
  zmq::socket_t Isend(*(m_data->context,ZMQ_REQ);
  std::stringstream connection;
  int port;
  m_variables.Get("remote_port",port);
  connection<<"tcp://127.0.0.1:"<<port;
  Isend.connect(connection.str());
  std::cout<<"s2"<<std::endl;


  zmq::pollitem_t items [] = {
    { Isend, 0, ZMQ_POLLOUT, 0 },
  };




  Store rr;                               

  rr.Set("msg_type","Command");
  rr.Set("msg_value","Stop");
  std::cout<<"s3"<<std::endl;


  std::string tmp="";
  rr>>tmp;
  zmq::message_t send(tmp.length()+1);
  snprintf ((char *) send.data(), tmp.length()+1 , "%s" ,tmp.c_str()) ;
  std::cout<<"s4"<<std::endl;
  printf("sd4");

  zmq::poll(&items [0], 1, -1);
  std::cout<<"s4.1"<<std::endl;
  printf("sd4.1");
  if ((items [0].revents & ZMQ_POLLOUT)) {
    std::cout<<"s4.2"<<std::endl;
    printf("sd4.2");
  Isend.send(send);
  std::cout<<"s5"<<std::endl;
  printf("sd5");

  zmq::message_t message;
  Isend.recv (&message);
  std::istringstream iss(static_cast<char*>(message.data()));
  //std::cout<<iss.str()<<std::endl;


  }
  zmq::poll(&items [0], 1, -1);

  if ((items [0].revents & ZMQ_POLLOUT)) {




  //std::cout<<"Sending Quit"<<std::endl;
  


  rr.Set("msg_type","Command");
  rr.Set("msg_value","Quit");

   
  
  tmp="";
  rr>>tmp;
  zmq::message_t send2(tmp.length()+1);
  snprintf ((char *) send2.data(), tmp.length()+1 , "%s" ,tmp.c_str()) ;
  std::cout<<"s6"<<std::endl;
  
Isend.send(send2);
 std::cout<<"s7"<<std::endl;

  //std::cout<<"quit sent"<<std::endl;

  zmq::message_t message2;
  Isend.recv (&message2);
  std::istringstream iss2(static_cast<char*>(message2.data()));
  std::cout<<iss2.str()<<std::endl;
  std::cout<<"s8"<<std::endl;
  //  sleep(5);
  }  
  //std::cout<<"before join"<<std::endl;
  //h1->Join();
  pthread_join(thread, NULL);
  std::cout<<"s9"<<std::endl;
 
  //std::cout<<"after join"<<std::endl;
  //delete Isend;
		      //Isend=0;
  std::cout<<"s10"<<std::endl;

  // delete h1;
  // h1=0;

  //  delete args;
  //args=0;
  std::cout<<"s11"<<std::endl;


  return true;
}


void* ThreadedSubToolChain::SubThread(void* arg){


 ToolChain_thread_args* args= static_cast<ToolChain_thread_args*>(arg);

   ToolChain TC(args->ToolChainConfig);
 //     ToolChain TC();
    //    TC.~ToolChain();
   std::cout<<"exiting thread"<<std::endl;   

   delete args;
   args=0;
   pthread_exit(NULL);
  
}
