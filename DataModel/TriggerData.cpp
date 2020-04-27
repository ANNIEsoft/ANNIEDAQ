#include "TriggerData.h"

void bencleanup2 (void *data, void *hint) { 
}
void bencleanup2b (void *data, void *hint) { 

  if(hint!=0){
    TriggerData* tmp=reinterpret_cast<TriggerData*>(hint);
    delete tmp;
  }
  
  
}

TriggerData::~TriggerData(){

}

void  TriggerData::Send(zmq::socket_t *socket, int flag, TriggerData* hint){
  
  //std::cout<<"d0"<<std::endl;
  zmq::message_t ms1(&FirmwareVersion,sizeof FirmwareVersion, bencleanup2);
  //std::cout<<"d0.1 FV="<< FirmwareVersion<<std::endl;  
  zmq::message_t ms2(&SequenceID,sizeof SequenceID, bencleanup2);
  //std::cout<<"d0.2 SID="<<SequenceID<<std::endl;
  
  zmq::message_t ms3(&EventSize,sizeof EventSize, bencleanup2);
  //std::cout<<"d0.3 ES="<<EventSize<<std::endl;
  
  //  zmq::message_t ms4(&TriggerSize,sizeof TriggerSize, bencleanup2);  
  zmq::message_t ms4(&TimeStampSize,sizeof TimeStampSize, bencleanup2);
//std::cout<<"d0.4 TS="<<TriggerSize<<std::endl;
  
  zmq::message_t ms5(&FIFOOverflow,sizeof FIFOOverflow, bencleanup2);
  //std::cout<<"d0.5 FO="<<FIFOOverflow<<std::endl;
  
  zmq::message_t ms6(&DriverOverflow,sizeof DriverOverflow, bencleanup2);
  //std::cout<<"d0.6 D0="<<DriverOverflow<<std::endl;

  socket->send(ms1,ZMQ_SNDMORE);
  socket->send(ms2,ZMQ_SNDMORE);
  socket->send(ms3,ZMQ_SNDMORE);
  socket->send(ms4,ZMQ_SNDMORE);
  socket->send(ms5,ZMQ_SNDMORE);
  socket->send(ms6,ZMQ_SNDMORE);                              
  
  //
  //std::cout<<"triggercounts snd size check = "<<triggerCounts.size()<<" card = "<< CardID<<std::endl;
  //std::cout<<"data.size = "<<Data.size()<<std::endl;
  //std::cout<<"d0.5"<<std::endl;

  //zmq::message_t ms6(&(triggerCounts.at(0)), sizeof(uint64_t)*triggerCounts.size(), bencleanup2);
  //std::cout<<"d0.6"<<std::endl;

  // zmq::message_t ms7(&(Rates.at(0)), sizeof(uint32_t)*Rates.size(), bencleanup2); 
  //std::cout<<"d0.7"<<std::endl;
  
  //  zmq::message_t ms1(&CardID,sizeof CardID, bencleanup2);
  //std::cout<<"d0.8"<<std::endl;

  // std::cout<<"CardID = "<<CardID<<std::endl;
  //zmq::message_t ms9(&channels,sizeof channels, bencleanup2);
  //std::cout<<"d0.9"<<std::endl;

  //  zmq::message_t ms10(&buffersize,sizeof buffersize, bencleanup2);
  //std::cout<<"d0.10"<<std::endl;

  // std::cout<<"buffersize = "<<buffersize<<std::endl;
  //std::cout<<"fullbuffsize = "<<fullbuffsize<<std::endl;
  //zmq::message_t ms11(&eventsize,sizeof eventsize, bencleanup2);
  //std::cout<<"d0.11"<<std::endl;

  //zmq::message_t ms3(&FirmwareVersion,sizeof FirmwareVersion, bencleanup2);
  size7=EventIDs.size(); 
  //std::cout<<"size7="<<size7<<std::endl; 
  zmq::message_t ms7a(&size7,sizeof size7, bencleanup2);
  if(size7>0){
    //std::cout<<"size7>0 = "<<size7<< std::endl;
    socket->send(ms7a,ZMQ_SNDMORE);
    zmq::message_t ms7b(&(EventIDs.at(0)), sizeof(uint16_t)*EventIDs.size(), bencleanup2);
    socket->send(ms7b,ZMQ_SNDMORE);  
  }
  else socket->send(ms7a,ZMQ_SNDMORE);
  
  
  //std::cout<<"d0.7"<<std::endl;
  size8=EventTimes.size(); 
  //std::cout<<"size8="<<size8<<std::endl;
  zmq::message_t ms8a(&size8,sizeof size8, bencleanup2);
  if(size8>0){
    //std::cout<<"size8>0 = "<<size8<< std::endl;
    socket->send(ms8a,ZMQ_SNDMORE);
    zmq::message_t ms8b(&(EventTimes.at(0)), sizeof(uint64_t)*EventTimes.size(), bencleanup2);
    socket->send(ms8b,ZMQ_SNDMORE);
  }
  else socket->send(ms8a,ZMQ_SNDMORE);
  
  /*
  
  //std::cout<<"d0.8"<<std::endl;
  size9=TriggerMasks.size();
  //std::cout<<"size9="<<size9<<std::endl; 
  zmq::message_t ms9a(&size9,sizeof size9, bencleanup2);  
  if(size9>0){
    //std::cout<<"size9>0 = "<<size9<< std::endl;
    socket->send(ms9a,ZMQ_SNDMORE);
    zmq::message_t ms9b(&(TriggerMasks.at(0)), sizeof(uint32_t)*TriggerMasks.size(), bencleanup2);
    socket->send(ms9b,ZMQ_SNDMORE);
  }
  else socket->send(ms9a,ZMQ_SNDMORE);
  
  
  //std::cout<<"d0.9"<<std::endl;
  size10=TriggerCounters.size();
  //std::cout<<"size10="<<size10<<std::endl; 
  if(size10>0){
    zmq::message_t ms10a(&size10,sizeof size10, bencleanup2);    
    //std::cout<<"size10>0 = "<<size10<< std::endl;
    socket->send(ms10a,ZMQ_SNDMORE);
    zmq::message_t ms10b(&(TriggerCounters.at(0)), sizeof(uint32_t)*TriggerCounters.size(), bencleanup2b,hint);
    socket->send(ms10b,flag);
  }
  else{
    zmq::message_t ms10a(&size10,sizeof size10, bencleanup2b,hint);
    socket->send(ms10a,flag);
  }
  */


  //std::cout<<"d0.9"<<std::endl;
  size10=TimeStampData.size();
  //std::cout<<"size10="<<size10<<std::endl; 
  if(size10>0){
    zmq::message_t ms10a(&size10,sizeof size10, bencleanup2);    
    //std::cout<<"size10>0 = "<<size10<< std::endl;
    socket->send(ms10a,ZMQ_SNDMORE);
    zmq::message_t ms10b(&(TimeStampData.at(0)), sizeof(uint32_t)*TimeStampData.size(), bencleanup2b,hint);
    socket->send(ms10b,flag);
  }
  else{
    zmq::message_t ms10a(&size10,sizeof size10, bencleanup2b,hint);
    socket->send(ms10a,flag);
  }

  
  //std::cout<<"d0.10"<<std::endl;
  
  //  std::cout<<"d0.12"<<std::endl;
  
  //  std::cout<<"data.size = "<<Data.size()<<std::endl;
  // std::cout<<"data.size2 = "<<Data.size()*sizeof(uint16_t)<<std::endl;
  //std::cout<<"d1"<<std::endl;  
/*  for (int i=0;i<Data.size();i++){

if(Data.at(i)==0)    std::cout<<" data.at("<<i<<")="<<Data.at(i);
  }
  std::cout<<std::endl;
  */
  //zmq::message_t ms8(&fullbuffsize,sizeof fullbuffsize, bencleanup2);
  //std::cout<<"sending"<<std::endl;
  //std::cout<<"sent"<<std::endl;

  // socket->send(ms11,ZMQ_SNDMORE);
  // socket->send(ms12);
  // std::cout<<"d2"<<std::endl;
  //std::cout<<" ben 1"<<std::endl;
  //for(int i=0;i<channels;i++){
  //zmq::message_t ms(&PMTID[0],&PMTID[channels]);
  // zmq::message_t ms(PMTID,sizeof(int)*channels,bencleanup2);
  //zmq::message_t ms(&PMTID[i],sizeof PMTID[i], bencleanup2);
    // std::cout<<" ben 2"<<std::endl;

    //socket->send(ms);
    //  }
    //std::cout<<" ben 3 "<< (sizeof(Data)/sizeof(uint16_t)) <<std::endl;

    //for(int i=0;i<fullbuffsize;i++){
    // Data[i];
    // }
    //std::cout<<"sizeof *Data "<<sizeof *Data <<std::endl;

    //std::cout<<"&Data[0] "<<&Data[0]<<std::endl;
    //std::cout<<"&Data[fullbuffsize] "<<&Data[fullbuffsize]<<std::endl;
     
    //std::vector<uint16_t> v(Data, Data + sizeof Data / sizeof Data[0]);
    //zmq::message_t mss(v.begin(),v.end());
  //    zmq::message_t mss(Data, sizeof(uint16_t)*fullbuffsize, bencleanup2); 

    //std::cout<<" ben 4"<<std::endl;
    
      //zmq::message_t ms(&Data[i],sizeof Data[i], bencleanup2);
  // socket->send(mss);
    //std::cout<<" ben 5"<<std::endl;

    //}

  //socket->send(ms6);


}

void TriggerData::Receive(zmq::socket_t *socket){
  /*
  int more;                                                                   
  size_t more_size = sizeof (more);                                           
  // Receive->getsockopt(ZMQ_RCVMORE, &more, &more_size);                     
                                                                                
  zmq::message_t message;                                                     
  //        std::cout<<"about to get message"<<std::endl;                     
                                                                                
  while (true){                                
    IF(Receive->recv(&message)){   
  */  
   /*
  zmq::message_t message;

  socket->recv(&message);
  LastSync=*(reinterpret_cast<uint64_t*>(message.data()));
  socket->recv(&message);
  SequenceID=*(reinterpret_cast<int*>(message.data()));
  socket->recv(&message);
  StartTimeSec=*(reinterpret_cast<int*>(message.data()));
  socket->recv(&message);
  StartTimeNSec=*(reinterpret_cast<int*>(message.data()));
  socket->recv(&message);
  StartCount=*(reinterpret_cast<uint64_t*>(message.data()));
  socket->recv(&message);
  //std::cout<<"triggercounts rec size check = "<<message.size()<<std::endl;
  triggerCounts.resize(message.size());
  triggerNumber=message.size();
  std::memcpy(&triggerCounts[0], message.data(), message.size());
  socket->recv(&message);
  Rates.resize(message.size());
  std::memcpy(&Rates[0], message.data(), message.size());
  socket->recv(&message);
  CardID=*(reinterpret_cast<int*>(message.data()));
  std::cout<<"CardID = "<<CardID<<std::endl;
  socket->recv(&message);
  channels=*(reinterpret_cast<int*>(message.data()));
  socket->recv(&message);
  buffersize=*(reinterpret_cast<int*>(message.data()));
  socket->recv(&message);
  eventsize=*(reinterpret_cast<int*>(message.data()));
  socket->recv(&message);
  Data.resize(message.size());
  fullbuffsize=message.size();
  std::memcpy(&Data[0], message.data(), message.size());
  */  
      /*
      Receive->getsockopt(ZMQ_RCVMORE, &more, &more_size);                    
          
}
                                                        
    if(more==0) break;
      */


}
