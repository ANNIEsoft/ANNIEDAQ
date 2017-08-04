#include "Monitoring2.h"

Monitoring2::Monitoring2():Tool(){}


bool Monitoring2::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  //std::string MonitorPort;
  // m_variables.Get("MonitorLevel",MonitoringLevel);

  Isend= new zmq::socket_t(*(m_data->context),ZMQ_PUB);
  Isend->bind("inproc://PlotThread");

 

  //   args=new monitor_thread_args(m_data->context,*(m_variables["OutputPath"]),Plots);
  
  //  h1 = new TThread("h1", Monitoring2::MonitorThread, args);
  //h1->Run();
  
  ////////// making plot threads ///////
  std::vector<Plot_thread*> Plots;
  Plot_thread* tmp= new Plot_thread(m_data->context, *(m_variables["OutputPath"]), "type", 20);
  tmp->h1=new TThread("type", Monitoring2::PlotThread, tmp);
  tmp->h1->Run();
  Plots.push_back(tmp);
  
  ////////////
    
  m_data->InfoTitle="MonitoringVariables";
  m_variables>>m_data->InfoMessage;
  m_data->GetTTree("RunInformation")->Fill();

  return true;

}


bool Monitoring2::Execute(){
  if(m_data->Restart==1)Finalise();
  else if(m_data->Restart==2)Initialise("",*m_data);
  else{
    if( m_data->triggered){
      
      //std::cout<<"m_data->NumEvents "<<m_data->NumEvents<<std::endl;
      //std::cout<<"MonitoringLevel "<<MonitoringLevel<<std::endl;
      //std::cout<<"(m_data->NumEvents % MonitoringLevel) "<<(m_data->NumEvents%MonitoringLevel)<<std::endl;
      
      //      if ((m_data->NumEvents%MonitoringLevel)==0){
	// std::cout<<"in"<<std::endl;   
	std::stringstream data;
	data<<"Data "<<m_data->carddata.size()<<" ";
	
	for(int i=0;i<m_data->carddata.size();i++){
	  data<<m_data->carddata.at(i)<<" ";
	  m_data->carddata.at(i)=0;
	}
	//  std::cout<<"message sending = "<<data.str()<<std::endl;
	zmq::message_t message(data.str().length()+1);
	
	snprintf ((char *) message.data(), data.str().length()+1 , "%s" ,data.str().c_str()) ;
	Isend->send(message);
	//std::cout<<"out"<<std::endl; 
	//      }
      
    }
  }
  return true;
}


bool Monitoring2::Finalise(){
  
  
  zmq::message_t message(256);
  snprintf ((char *) message.data(), 256 , "%s" ,"Quit") ;
  Isend->send(message);
  
  //  (void) pthread_join(thread, NULL);
  
  for(int plot=0;plot<Plots.size();plot++){
    
    Plots.at(plot)->h1->Join();  
    
    delete Plots.at(plot)->h1;
    Plots.at(plot)->h1=0;
  
    delete Plots.at(plot);
    Plots.at(plot)=0;
  }

  Plots.clear();

  delete Isend;
  Isend=0;
  
  //  delete args;
  //args=0;

  return true;
}

/*
void* Monitoring2::MonitorThread(void* arg){
  
  //std::cout<<"d1"<<std::endl;
  monitor_thread_args* args= static_cast<monitor_thread_args*>(arg);  

  std::string outpath=args->outputpath;
  zmq::socket_t Ireceive (*(args->context), ZMQ_PAIR);
  Ireceive.connect("inproc://MonitorThread");
  
  //  std::vector<CardData*> carddata;


  bool running=true;
  
  while (running){
    //std::cout<<"d4"<<std::endl;

    
    zmq::message_t comm;
    Ireceive.recv(&comm);
	
    std::istringstream iss(static_cast<char*>(comm.data()));
    std::string arg1="";
    iss>>arg1;

    //std::cout<<"d5"<<std::endl;
    

    if(arg1=="Data"){

      CardData* carddata;
      int size=0;
      iss>>size;  
      
      //freqplots.clear();
  //std::cout<<"d6"<<std::endl;
  
      for(int i=0;i<size;i++){
	//std::cout<<"d7"<<std::endl;

	long long unsigned int pointer;
	iss>>std::hex>>pointer;
	
	carddata=(reinterpret_cast<CardData *>(pointer));
	
      }
    
    }
    
    
    
    else if(arg1=="Quit"){
      
      running=false;
      //std::cout<<"d37"<<std::endl;
    }
    
  }
  
//std::cout<<"d38"<<std::endl;
}

*/
void* Monitoring2::PlotThread(void* arg){

  //std::cout<<"d1"<<std::endl;
  Plot_thread* args= static_cast<Plot_thread*>(arg);

  std::string outpath=args->outputpath;
  zmq::socket_t Ireceive (*(args->context), ZMQ_SUB);
  Ireceive.connect("inproc://PlotThread");
  Ireceive.setsockopt(ZMQ_SUBSCRIBE, "", 0);

  Plot* plot= PlotFactory(args->type);
  plot->Initialise(args->outputpath);
  
  bool running=true;

  long eventcounter=0;  

  while (running){


    zmq::message_t comm;
    Ireceive.recv(&comm);

    std::istringstream iss(static_cast<char*>(comm.data()));
    std::string arg1="";
    iss>>arg1;


    if(arg1=="Data"){                                                                                        
      if(eventcounter%args->frequency){                                           
	std::vector<CardData*> carddata;                
	int size=0;                                                                                                                                                                                               	iss>>size;                                                                                                                                                                                 
	for(int i=0;i<size;i++){                                                                                                                                                                   
	  long long unsigned int pointer;                                                                                                                                                
	  iss>>std::hex>>pointer;                                                                                                                                                                 
	  CardData* tmp;                                                                                                                                                                                
	  tmp=(reinterpret_cast<CardData *>(pointer));                                                                                                                                                          
	  carddata.push_back(tmp);                                                                                                                                                 
	}                                                                                                                                                                                                            plot->Execute(carddata);
	
      }                                                                             
    }             
    
    else if(arg1=="Quit"){
      
      running=false;
      
    }
    
    
  }
  
  plot.Finalise();
  delete plot;
  plot=0;
  
}
