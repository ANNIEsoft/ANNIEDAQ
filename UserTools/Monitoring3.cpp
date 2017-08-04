#include "Monitoring3.h"

Monitoring::Monitoring():Tool(){}


bool Monitoring::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;
 
  //////////////////////////Adding plots  ///////////////////// 
  int MonitoringLevel=0;
  m_variables.Get("MonitorLevel",MonitoringLevel);
  
  MonitoringLevels.push_back(MonitoringLevel);
  ///////////////////////////////////////////////

  Isend= new zmq::socket_t(*(m_data->context),ZMQ_PAIR);
  Isend->bind("inproc://MonitorThread");

  //std::cout<<"initialised monitor "<<MonitoringLevel<<std::endl;

  args=new monitor_thread_args(m_data->context, &m_variables);
  // pthread_create (&thread, NULL, Monitoring::MonitorThread, args);
  h1 = new TThread("h1", Monitoring::MonitorThread, args);
  h1->Run();
  //  int i=system("rm /data/monitoringplots/* &");
  
  m_data->InfoTitle="MonitoringVariables";
  m_variables>>m_data->InfoMessage;
  m_data->GetTTree("RunInformation")->Fill();

  return true;

}


bool Monitoring::Execute(){
  if(m_data->Restart==1)Finalise();
  else if(m_data->Restart==2)Initialise("",*m_data);
  else{
    if( m_data->triggered){
      
      bool monitorsend=false;
      
      //std::cout<<"in execute"<<std::endl;

      for(int i=0;i<MonitoringLevels.size();i++){
	//std::cout<<"numevents="<<m_data->NumEvents<<" monitorlevel="<<MonitoringLevels.at(i)<<" %="<<(m_data->NumEvents%MonitoringLevels.at(i))<<std::endl;
	if(MonitoringLevels.at(i)!=0){
	  if ((m_data->NumEvents%MonitoringLevels.at(i))==0){
	    //std::cout<<"monitorsend set to true"<<std::endl;
	    monitorsend=true;
	  }
	}  
      }

	//std::cout<<"monitorsend="<<monitorsend<<std::endl;
      
      if (monitorsend){
	 //std::cout<<"in monitor send"<<std::endl;   
	std::stringstream data;
	data<<"Data "<<m_data->carddata.size()<<" ";
	
	for(int i=0;i<m_data->carddata.size();i++){
	  data<<m_data->carddata.at(i)<<" ";
	  m_data->carddata.at(i)=0;
	}
	
	data<<m_data->NumEvents;
	
	//std::cout<<"message sending = "<<data.str()<<std::endl;
	zmq::message_t message(data.str().length()+1);
	
	snprintf ((char *) message.data(), data.str().length()+1 , "%s" ,data.str().c_str()) ;
	Isend->send(message);
	//std::cout<<"out"<<std::endl; 
      }
      
    }
  }

  return true;
}

  
bool Monitoring::Finalise(){


  zmq::message_t message(256);
  snprintf ((char *) message.data(), 256 , "%s" ,"Quit") ;
  Isend->send(message);
  
  //  (void) pthread_join(thread, NULL);
  h1->Join();  

  delete h1;
  h1=0;

  delete Isend;
  Isend=0;

  delete args;
  args=0;

  MonitoringLevels.clear();

  return true;
}


void* Monitoring::MonitorThread(void* arg){
  
  //std::cout<<"d1"<<std::endl;
  monitor_thread_args* args= static_cast<monitor_thread_args*>(arg);  

  zmq::socket_t Ireceive (*(args->context), ZMQ_PAIR);
  Ireceive.connect("inproc://MonitorThread");

  std::map<int, Plot*> plots;

  //////////////////////////////////Adding plots /////////////////
  int monlevel=0;
  args->variables->Get("MonitorLevel",monlevel);
  Plot* bb= new OriginalPlots();

  plots[monlevel]=bb;
  ///////////////////////////////////////////////////////////////


  //std::cout<<"thread mon level"<<monlevel<<std::endl;

  for (std::map<int,Plot*>::iterator it=plots.begin(); it!=plots.end(); ++it){
   
    if(it->first!=0) it->second->Initialise(args->variables);
  
  }
 
  bool running=true;
  
  //std::cout<<"int thread"<<std::endl;

  while (running){
    //std::cout<<"d4"<<std::endl;
    try{
      
      zmq::message_t comm;
      Ireceive.recv(&comm);
      
      std::istringstream iss(static_cast<char*>(comm.data()));
      //std::cout<<"thread received messaage="<<iss.str()<<std::endl;
      std::string arg1="";
      iss>>arg1;
      
      //std::cout<<"d5"<<std::endl;
      
      
      if(arg1=="Data"){
	
	//std::cout<<"thread got data"<<std::endl;
	
	std::vector<CardData*> carddatavec;
	CardData* carddata;
	int size=0;
	iss>>size;  
	
	
	//std::cout<<"d6"<<std::endl;
	
	for(int i=0;i<size;i++){
	  //std::cout<<"d7"<<std::endl;
	  
	  long long unsigned int pointer;
	  iss>>std::hex>>pointer;
	  
	  carddata=(reinterpret_cast<CardData *>(pointer));
	  carddatavec.push_back(carddata);
	  
	  
	  
	} /// size i
	
	long eventnum=0;
	iss>>std::dec>>eventnum;
	
	
	for (std::map<int,Plot*>::iterator it=plots.begin(); it!=plots.end(); ++it){
	  //std::cout<<"in thread evntnum="<<eventnum<<" %="<<(eventnum%it->first)<<std::endl;
	  if(it->first!=0) {
	    if((eventnum%it->first)==0)it->second->Execute(&carddatavec);
	  }
	}
	
	for(int i=0;i<carddatavec.size();i++){
	  delete carddatavec.at(i);
	  carddatavec.at(i)=0;
	}
	
	carddatavec.clear();
	
      }
      
      
      
      else if(arg1=="Quit"){
	
	for (std::map<int,Plot*>::iterator it=plots.begin(); it!=plots.end(); ++it){
	  
	  if(it->first!=0) it->second->Finalise();
	  
	}
	
	plots.clear();
	
	running=false;
	//std::cout<<"d37"<<std::endl;
      }
    }
    catch(...){
      std::cout<<"error in monitoring thread"<<std::endl;
    }
  }
  
  //std::cout<<"d38"<<std::endl;
}
