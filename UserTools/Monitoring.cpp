#include "Monitoring.h"

Monitoring::Monitoring():Tool(){}


bool Monitoring::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  std::string MonitorPort;
  m_variables.Get("MonitorLevel",MonitoringLevel);

  Isend= new zmq::socket_t(*(m_data->context),ZMQ_PUSH);
  Isend->bind("inproc://MonitorThread");

  args=new monitor_thread_args(m_data->context,*(m_variables["OutputPath"]));
  pthread_create (&thread, NULL, Monitoring::MonitorThread, args);
  
  return true;

}


bool Monitoring::Execute(){

  if( m_data->triggered){

    if ((m_data->NumEvents % MonitoringLevel)==0){
   
      std::stringstream data;
      data<<"Data "<<m_data->carddata.size()<<" ";

      for(int i=0;i<m_data->carddata.size();i++){
	data<<m_data->carddata.at(i)<<" ";
	m_data->carddata.at(i)=0;
      }

      zmq::message_t message(data.str().length()+1);
      
      snprintf ((char *) message.data(), data.str().length()+1 , "%s" ,data.str().c_str()) ;
      Isend->send(message);
      
    }
    
  }
  
  return true;
}


bool Monitoring::Finalise(){


  zmq::message_t message(256);
  snprintf ((char *) message.data(), 256 , "%s" ,"Quit") ;
  Isend->send(message);
  
  (void) pthread_join(thread, NULL);
  
  delete Isend;
  Isend=0;

  delete args;
  args=0;

  return true;
}


void* Monitoring::MonitorThread(void* arg){
  
  monitor_thread_args* args= static_cast<monitor_thread_args*>(arg);
  
  std::string outpath=args->outputpath;
  zmq::socket_t Ireceive (*(args->context), ZMQ_PULL);
  Ireceive.connect("inproc://MonitorThread");
  
  //  std::vector<CardData*> carddata;
  std::vector<TH1I> freqplots;
  TCanvas c1("c1","c1",600,400);
  
  
  bool running=true;
  bool init=true;    
  
  while (running){
    
    
    zmq::message_t comm;
    Ireceive.recv(&comm);
	
    std::istringstream iss(static_cast<char*>(comm.data()));
    std::string arg1="";
    iss>>arg1;
    
    if(arg1=="Data"){
      TH2I EventDisplay ("Event Display", "Event Display", 20, 0, 20, 20, 0, 20);
      std::vector<TH1I> temporalplots;
      CardData* carddata;
      int size=0;
      iss>>size;  
      
      //freqplots.clear();
      
      for(int i=0;i<size;i++){
	
	long long unsigned int pointer;
	iss>>std::hex>>pointer;
	
	carddata=(reinterpret_cast<CardData *>(pointer));
	
	if(init){
	  for(int j=0;j<carddata->channels;j++){
	    std::stringstream tmp;
	    tmp<<"Channel "<<(i*4)+j<<" frequency";
	    TH1I tmpfreq(tmp.str().c_str(),tmp.str().c_str(),600,0,600);
	    freqplots.push_back(tmpfreq);
	  }
	  if(i==size<-1)init=false;
	}

	for(int j=0;j<carddata->channels;j++){
	  std::stringstream tmp;
	  tmp<<"Channel "<<(i*4)+j<<" temporal";
	  
	  TH1I temporal(tmp.str().c_str(),tmp.str().c_str(),carddata->buffersize,0,carddata->buffersize);
	  long sum=0;
	
	for(int k=0;k<carddata->buffersize;k++){
	  sum+=carddata->Data[(j*carddata->buffersize)+k];  
	  freqplots.at(i).Fill(carddata->Data[(j*carddata->buffersize)+k]);
	  temporal.SetBinContent(k,carddata->Data[(j*carddata->buffersize)+k]);	      
	}
	
	
	temporalplots.push_back(temporal);
	EventDisplay.SetBinContent((i*4)+j,(i*4)+j,sum);
	}

	delete carddata;
	
      }
      
      
      for(int i=0;i<freqplots.size();i++){
	if(i==0)freqplots.at(i).Draw();
	else freqplots.at(i).Draw("same");
	
      }
      std::stringstream tmp;
      tmp<<outpath<<"freq.jpg";
      c1.SaveAs(tmp.str().c_str());
      
      for(int i=0;i<size;i++){
	if(i==0)temporalplots.at(i).Draw();
	else temporalplots.at(i).Draw("same");
	
      }	  
      std::stringstream tmp2;
      tmp2<<outpath<<"temporal.jpg";
      c1.SaveAs(tmp2.str().c_str());
      
      EventDisplay.Draw();
      std::stringstream tmp3;
      tmp3<<outpath<<"EventDisplay.jpg";
      c1.SaveAs(tmp3.str().c_str());
      
    }
    
    
    
    else if(arg1=="Quit"){
      freqplots.clear();
      running=false;
      
    }
    
  }
  
  
}
