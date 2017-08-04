#include "Logger.h"

Logger::Logger():Tool(){}


bool Logger::Initialise(std::string configfile, DataModel &data){
  
  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_variables.Get("log_port",m_log_port);
  
  /////////////setting up log receiver //////////
  args=new logger_args();
  args->context=m_data->context;
  args->log_port=m_log_port;

  pthread_create (&thread, NULL, Logger::LogThread, args);

  Thread =  new zmq::socket_t(*m_data->context, ZMQ_PAIR);
  Thread->connect("inproc://logger");
  ////////////////////////////////////////////
  
  /////////////adding log store to servicec discovery /////////////

  zmq::socket_t Ireceive (*m_data->context, ZMQ_PUSH);
  Ireceive.connect("inproc://ServicePublish");

  boost::uuids::uuid m_UUID;
  m_UUID = boost::uuids::random_generator()();
  std::stringstream test;
  test<<"Add "<< "LogStore "<<m_UUID<<" 24010 "<<"0";
  zmq::message_t send(test.str().length()+1);
  snprintf ((char *) send.data(), test.str().length()+1 , "%s" ,test.str().c_str()) ;
  Ireceive.send(send);




  ///////////////////////////////////

  


  m_data->InfoTitle="LoggerVariables";
  m_variables>>m_data->InfoMessage;
  m_data->GetTTree("RunInformation")->Fill();
  

  return true;
}

bool Logger::Execute(){
  if(m_data->Restart==1)Finalise();
  else if(m_data->Restart==2)Initialise("",*m_data);
  return true;

}

bool Logger::Finalise(){
  
  zmq::message_t command(5);
  snprintf ((char *) command.data(), 5 , "%s" ,"Quit") ;
  if(Thread->send(command)){
    
    zmq::message_t catlog;
    if(Thread->recv(&catlog)){
      std::istringstream ss(static_cast<char*>(catlog.data()));

      m_data->InfoTitle="RemoteLogs";
      m_data->InfoMessage=ss.str();
      m_data->GetTTree("RunInformation")->Fill();
    
    }
  }

  pthread_join(thread, NULL);
  
  delete Thread;
  Thread=0;
  delete args;
  args=0;

  m_data->InfoTitle="LocalLog";
  std::string locallog;
  std::string catlog;
  m_data->vars.Get("log_local_path",locallog); 
  std::string line;
    ifstream myfile (locallog.c_str());
  if (myfile.is_open())
    {
      while ( getline (myfile,line) )
	{
	  catlog+=line+'\n';
	}
    }
  m_data->InfoMessage=catlog;
  m_data->GetTTree("RunInformation")->Fill();
  

  /////////////remove service //////////////

  zmq::socket_t Ireceive (*m_data->context, ZMQ_PUSH);
  Ireceive.connect("inproc://ServicePublish");
  std::stringstream test;
  test<<"Delete "<< "LogStore ";
  zmq::message_t send(test.str().length()+1);
  snprintf ((char *) send.data(), test.str().length()+1 , "%s" ,test.str().c_str()) ;
  Ireceive.send(send);

  ///////////////////////////////////////
  
  

  return true;
}




void* Logger::LogThread(void* arg){

  logger_args* args= static_cast<logger_args*>(arg);


  zmq::socket_t  Thread(*(args->context), ZMQ_PAIR);
  Thread.bind("inproc://logger");

  zmq::socket_t  LogReceiver(*(args->context), ZMQ_PULL);
  std::stringstream tmp;
  tmp<<"tcp://*:"<<args->log_port;
  //std::cout<<"connection address = "<<tmp.str()<<std::endl;
  //LogReceiver->bind(tmp.str().c_str());
  //  LogReceiver->setsockopt(ZMQ_SUBSCRIBE,"");
  LogReceiver.bind(tmp.str().c_str());
  //  printf("connection made\n");

  std::vector<std::string> log_messages;

  zmq::pollitem_t items [] = {
    { LogReceiver, 0, ZMQ_POLLIN, 0 },
    { Thread, 0, ZMQ_POLLIN, 0 }
  };
  
  bool running=true;
  
  while (running){
    
    zmq::poll (&items[0], 2, 1000);
    
    // printf("finnished poll \n");
    
    if (items[0].revents & ZMQ_POLLIN) {
      //   printf("in receive if \n");   
      zmq::message_t Rmessage;
      if(  LogReceiver.recv (&Rmessage)){
	// printf("got a message \n");
	std::istringstream ss(static_cast<char*>(Rmessage.data()));
	
	//std::cout<<ss.str()<<std::flush;
		
	Store bb;
	
	bb.JsonPaser(ss.str());
	
	log_messages.push_back(*(bb["msg_value"]));
			       
      }
    }
    
    
    if (items[1].revents & ZMQ_POLLIN) {
      zmq::message_t command;
      if(  Thread.recv (&command)){
	std::istringstream ss(static_cast<char*>(command.data()));
	if(ss.str()=="Quit"){
	  running=false;
	  
	  std::string catlog;
	  for(int i=0;i<log_messages.size();i++){
	    catlog+=log_messages.at(i)+'\n';
	  }
	  
	  zmq::message_t logs(catlog.length()+1);
	  snprintf ((char *) logs.data(), catlog.length()+1 , "%s" ,catlog.c_str()) ;
	  Thread.send(logs);
	  
	}
      }
      
      
    }
    
  } 
  
  pthread_exit(NULL);
  
}

