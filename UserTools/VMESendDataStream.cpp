#include "VMESendDataStream.h"

VMESendDataStream::VMESendDataStream():Tool(){}


bool VMESendDataStream::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_variables.Get("port",m_port);
  m_variables.Get("trigport",m_trigport);
  
  Send= new zmq::socket_t(*(m_data->context), ZMQ_PUSH);
  TrigSend= new zmq::socket_t(*(m_data->context), ZMQ_PUSH);
  //  int a=12000;
  //Send->setsockopt(ZMQ_SNDTIMEO, a);
  //Send->setsockopt(ZMQ_RCVTIMEO, a);

  DataRecv= new zmq::socket_t(*(m_data->context), ZMQ_PULL);
  DataRecv->connect("inproc://ADCDataSend");

  TrigDataRecv= new zmq::socket_t(*(m_data->context), ZMQ_PULL);
  TrigDataRecv->connect("inproc://TrigDataSend");


  std::stringstream tmp;
  tmp<<"tcp://*:"<<m_port;
  Send->bind(tmp.str().c_str());

  std::stringstream tmp2;
  tmp2<<"tcp://*:"<<m_trigport;
  TrigSend->bind(tmp2.str().c_str());

  out[0].socket=*Send;
  out[0].fd=0;
  out[0].events=ZMQ_POLLOUT;
  out[0].revents=0;

  out2[0].socket=*TrigSend;
  out2[0].fd=0;
  out2[0].events=ZMQ_POLLOUT;
  out2[0].revents=0;
  
  Datain[0].socket=*DataRecv;
  Datain[0].fd=0;
  Datain[0].events=ZMQ_POLLIN;
  Datain[0].revents=0;

  Datain[1].socket=*TrigDataRecv;
  Datain[1].fd=0;
  Datain[1].events=ZMQ_POLLIN;
  Datain[1].revents=0;

  datacount=0;
  trigcount=0;
  statusmsg.Set("Data",datacount);
  statusmsg.Set("Trig",trigcount);

  carddatavec.clear();
  triggerdatavec.clear();

  
  return true;
}


bool VMESendDataStream::Execute(){
  // std::cout<<"a1"<<std::endl;
  if(m_data->triggered){
    //std::cout<<"a2"<<std::endl;
    zmq::poll (&Datain[0], 2, 0);
    //std::cout<<"a3"<<std::endl;
    if (Datain[0].revents & ZMQ_POLLIN ) {
      //std::cout<<"got data "<<datacount<<std::endl;    
      zmq::message_t in;
      DataRecv->recv(&in);
      std::istringstream iss2(static_cast<char*>(in.data()));
      unsigned long arg2;
      iss2 >> std::hex >> arg2;
      std::vector<CardData>* tmp;
      tmp=reinterpret_cast<std::vector<CardData>* >(arg2);
      
      //	for(int k=0;k<tmp->size();k++){

      //  if(tmp->at(k).CardID>4000) std::cout<<"wegot one "<< tmp->at(k).CardID<<std::endl;
      //	}

      carddatavec.push_back(tmp);
      
      datacount++;      
      statusmsg.Set("Data",datacount);
      
      std::stringstream status;
      long trign;
      statusmsg.Get("Trig",trign);
      status<<"Data="<<datacount<<", Trig="<<trign;
      //std::cout<<"status = "<<status.str()<<std::endl;
      m_data->vars.Set("Status",status.str());
    }
    
    if (Datain[1].revents & ZMQ_POLLIN ) {
      //std::cout<<"got trig "<<trigcount<<std::endl;
      zmq::message_t in;
      TrigDataRecv->recv(&in);
      std::istringstream iss2(static_cast<char*>(in.data()));
      unsigned long arg2;
      iss2 >> std::hex >> arg2;
      TriggerData* tmp;
      tmp=reinterpret_cast<TriggerData*>(arg2);
      triggerdatavec.push_back(tmp);

      trigcount++;
      statusmsg.Set("Trig",trigcount);
 
       std::stringstream status;
      long datan;
      statusmsg.Get("Data",datan);
      status<<"Data="<<datan<<", Trig="<<trigcount;
      //std::cout<<"status = "<<status.str()<<std::endl;
      m_data->vars.Set("Status",status.str());


    }
    //std::cout<<"a4"<<std::endl;
    if(carddatavec.size()>0){
      //std::cout<<"a5 "<<carddatavec.size()<<std::endl;
      
      zmq::poll (&out[0], 1, 0);
      //std::cout<<"a6"<<std::endl;
      if (out[0].revents & ZMQ_POLLOUT ) {
	//std::cout<<"Sending data "<<carddatavec.size()<<std::endl;
	int size= carddatavec.at(0)->size();
	zmq::message_t cards(sizeof size);
	memcpy ( cards.data(), &size, sizeof size );
	//snprintf ((char *) cards.data(),  sizeof size , "%d" ,size) ;
	//std::cout<<"Sending card size = "<<m_data->carddata.size()<<std::endl;
	if(size>0)   Send->send(cards,ZMQ_SNDMORE);
	else Send->send(cards);
	//std::cout<<"Sent"<<std::endl;    
	
	for (int i=0;i<carddatavec.at(0)->size();i++){
	
	//  std::cout<<" card id= "<<m_data->carddata.at(i)->CardID<<std::endl;      
    //NEW ***************************************	  
    //Send->send(m_data->carddata.begin(),m_data->carddata.end());
       //m_data->carddata.at(i)
      // std::cout<<"Sending"<<std::endl;
	//std::cout<<"before poll 2 i="<<i<<std::endl;
	//zmq::poll (&out[0], 1, 0);
	//std::cout<<"after poll 2"<<std::endl;

	//if (out[0].revents & ZMQ_POLLOUT) {    
	  //sd::cout<<"sending card data"<<std::endl;
	  //for(int k=0;k<carddatavec.at(0)->size();k++){

	  //if(carddatavec.at(0)->at(k).CardID>4000) std::cout<<"wegot one "<< carddatavec.at(0)->at(k).CardID<<std::endl;
	  //}

	  if( i <(carddatavec.at(0)->size()-1)){
	    //sd::cout<<"sendmore "<<i<<std::endl;
	    
	    carddatavec.at(0)->at(i).Send(Send, ZMQ_SNDMORE);
	  }
	  else{
	    carddatavec.at(0)->at(i).Send(Send,0,carddatavec.at(0));
	    //sd::cout<<"send "<<i<<std::endl;
	  }



	}
	//delete carddatavec.at(0);
	carddatavec.at(0)=0;
	carddatavec.pop_front();
      }
    }
    
    
    //std::cout<<"a7"<<std::endl;
    if(triggerdatavec.size()>0){
      // std::cout<<"a8 "<<triggerdatavec.size()<<std::endl;
      //std::cout<<"before poll 3"<<std::endl;
      zmq::poll (&out2[0], 1, 0);
      //std::cout<<"a9"<<std::endl;  
    //std::cout<<"after poll 3"<<std::endl;  
      if (out2[0].revents & ZMQ_POLLOUT) {  
	//std::cout<<"sending trigger "<<triggerdatavec.size()<<std::endl;
	//std::cout<<"in trig send poll success"<<std::endl;
	triggerdatavec.at(0)->Send(TrigSend,0, triggerdatavec.at(0));
	//delete triggerdatavec.at(0);
	triggerdatavec.at(0)=0;
	triggerdatavec.pop_front(); 
	//std::cout<<"in trig sent"<<std::endl;
      }
    }
    
  }
  
  else{
      //std::cout<<"a10"<<std::endl;
    for(int i=0;i<carddatavec.size();i++){
      delete carddatavec.at(i);
      carddatavec.at(i)=0;
    }   
    carddatavec.clear();
    
    for (int i=0;i< triggerdatavec.size();i++){
      delete triggerdatavec.at(i);
      triggerdatavec.at(i)=0;
    }
    
    triggerdatavec.clear();
  
    statusmsg.Delete();
    datacount=0;
    trigcount=0;
    statusmsg.Set("Data",datacount);
    statusmsg.Set("Trig",trigcount);
    std::stringstream status;
    status<<"Data="<<datacount<<", Trig="<<trigcount;
    //std::cout<<"status = "<<status.str()<<std::endl;
    m_data->vars.Set("Status",status.str());
    
    // std::cout<<"a11"<<std::endl;
  }  
  //std::cout<<"a12"<<std::endl;
  return true;
}



bool VMESendDataStream::Finalise(){
  
  delete  Send;
  Send=0;

  delete TrigSend;
  TrigSend=0;

  delete DataRecv;
  DataRecv=0;

  delete TrigDataRecv;
  TrigDataRecv=0;

  //std::cout<<"a10"<<std::endl;
    for(int i=0;i<carddatavec.size();i++){
      delete carddatavec.at(i);
      carddatavec.at(i)=0;
    }   
    carddatavec.clear();
    
    for (int i=0;i< triggerdatavec.size();i++){
      delete triggerdatavec.at(i);
      triggerdatavec.at(i)=0;
    }
    
    triggerdatavec.clear();
  
  
  datacount=0;
  trigcount=0;
  statusmsg.Delete();

  std::string tmp="";
  m_data->vars.Set("Status",tmp);

  return true;
}

