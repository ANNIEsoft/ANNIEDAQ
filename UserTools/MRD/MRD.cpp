#include "ToolChain.h"
#include "MRD.h"


MRD::MRD():Tool(){}


bool MRD::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  args=new mrd_thread_args();
  args->context=m_data->context;
  m_variables.Get("log_mode",args->log_mode);
  m_variables.Get("log_path",args->log_path);
  m_variables.Get("log_service",args->log_service);
  m_variables.Get("log_port",args->log_port);
  m_variables.Get("verbose",args->verbose);
  m_variables.Get("errorlevel",args->errorlevel);
  m_variables.Get("kick_time",args->kick_time);
  m_variables.Get("toolsfile",args->toolsfile);
  m_variables.Get("remote_port",args->remote_port);
  m_variables.Get("multicastaddress",args->multicastaddress);
  m_variables.Get("multicastport",args->multicastport);

  //pthread_create (&thread, NULL, MRD::MRDThread, args);
  h1 = new TThread("h1", MRD::MRDThread, args);
  h1->Run();
  
  sleep(1);

  Isend= new zmq::socket_t(*(m_data->context),ZMQ_REQ);
  std::stringstream connection;
  int port;
  m_variables.Get("remote_port",port);
  connection<<"tcp://127.0.0.1:"<<port;
  Isend->connect(connection.str());


  //  std::istringstream iss(static_cast<char*>(message.data()));
  // command=iss.str();

  Store rr;
  //rr.JsonPaser(command);
 
  rr.Set("msg_type","Command");
  rr.Set("msg_value","Start");

  

  /*
  boost::posix_time::ptime t = boost::posix_time::microsec_clock::universal_time();
  std::stringstream isot;
  isot<<boost::posix_time::to_iso_extended_string(t) << "Z";

  msg_id++;
  Store bb;

  bb.Set("uuid",m_UUID);
  bb.Set("msg_id",msg_id);
  *bb["msg_time"]=isot.str();
  *bb["msg_type"]="Command Reply";
  bb.Set("msg_value",ExecuteCommand(command));

  */
  std::string tmp="";
  rr>>tmp;
  zmq::message_t send(tmp.length()+1);  
  snprintf ((char *) send.data(), tmp.length()+1 , "%s" ,tmp.c_str()) ;
  Isend->send(send);

  zmq::message_t message;
  Isend->recv (&message);
  std::istringstream iss(static_cast<char*>(message.data()));
  std::cout<<iss.str()<<std::endl;

  delete Isend;

  m_data->InfoTitle="MRDVariables";
  m_variables>>m_data->InfoMessage;
  m_data->GetTTree("RunInformation")->Fill();

  return true;
}


bool MRD::Execute(){
  if(m_data->Restart==1)Finalise();
  else if(m_data->Restart==2)Initialise("",*m_data);
  return true;
}


bool MRD::Finalise(){

  
  Isend= new zmq::socket_t(*(m_data->context),ZMQ_REQ);
  std::stringstream connection;
  int port;
  m_variables.Get("remote_port",port);
  connection<<"tcp://127.0.0.1:"<<port;
  Isend->connect(connection.str());

  Store rr;
                                                                                                                                                                           

  rr.Set("msg_type","Command");
  rr.Set("msg_value","Stop");


  std::string tmp="";
  rr>>tmp;
  zmq::message_t send(tmp.length()+1);
  snprintf ((char *) send.data(), tmp.length()+1 , "%s" ,tmp.c_str()) ;
  Isend->send(send);

  zmq::message_t message;
  Isend->recv (&message);
  std::istringstream iss(static_cast<char*>(message.data()));
  //std::cout<<iss.str()<<std::endl;

  //std::cout<<"Sending Quit"<<std::endl;
  
  rr.Set("msg_type","Command");
  rr.Set("msg_value","Quit");

   
  
  tmp="";
  rr>>tmp;
  zmq::message_t send2(tmp.length()+1);
  snprintf ((char *) send2.data(), tmp.length()+1 , "%s" ,tmp.c_str()) ;
  Isend->send(send2);
  //std::cout<<"quit sent"<<std::endl;

  zmq::message_t message2;
  Isend->recv (&message2);
  std::istringstream iss2(static_cast<char*>(message2.data()));
  //std::cout<<iss2.str()<<std::endl;

 
  //std::cout<<"before join"<<std::endl;
  h1->Join();
  //pthread_join(thread, NULL); 
  //std::cout<<"after join"<<std::endl;
  delete Isend;
  Isend=0;

  delete h1;
  h1=0;

  delete args;
  args=0;

  return true;
}



void* MRD::MRDThread(void* arg){

 mrd_thread_args* args= static_cast<mrd_thread_args*>(arg);

 //args->log_mode
 //args->log_path
 //args->log_service
 //args->log_port
 //args->verbose
 //args->errorlevel
 //args->kick_time
 //args->toolsfile
 //args->remote_port
 //args->multicastaddress
 //args->multicastport
  // std::string outpath=args->outputpath;
  //zmq::socket_t Ireceive (*(args->context), ZMQ_PAIR);
  //Ireceive.connect("inproc://MonitorThread");

   ToolChain MRDtc("./configfiles/MRDToolChain");
   
   /*
   ToolChain MRDtc(args->verbose,args->errorlevel,"MRDService",args->log_mode,args->log_path,args->log_service,args->log_port,-1,args->kick_time);
  


//std::string toolsfile="";
// config.Get("Tools_File",toolsfile);

  if(args->toolsfile!=""){
    std::ifstream file(args->toolsfile.c_str());
    std::string line;
    if(file.is_open()){

      while (getline(file,line)){
        if (line.size()>0){
          if (line.at(0)=='#')continue;
	  std::string name;
	  std::string tool;
	  std::string conf;
	  std::stringstream stream(line);

          if(stream>>name>>tool>>conf) MRDtc.Add(name,Factory(tool),conf); /// need to fix

        }

      }
    }
  
  
  file.close();
}
  MRDtc.Remote(args->remote_port,args->multicastaddress, args->multicastport);
   */

  //DummyTool dummytool;                                                                                                                                                                                          
  //tools.Add("DummyTool",&dummytool,"configfiles/DummyToolConfig");                                                                                                                                              
  //int portnum=24000;                                                                                                                                                                                            
  //  tools.Remote(portnum);                                                                                                                                                                                      
  //tools.Interactive();                                                                                                                                                                                          
  //  tools.Initialise();                                                                                                                                                                                         
  // tools.Execute();                                                                                                                                                                                             
  //tools.Finalise();           
  
   //  pthread_exit(NULL);
  
}
