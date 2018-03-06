#include "TriggerData.h"

void bencleanup2 (void *data, void *hint) {
  //  delete data;  
//  free (data);
}

void  TriggerData::Send(zmq::socket_t *socket){

  //std::cout<<"d0 FirmwareVersion = "<<FirmwareVersion<<std::endl;
  zmq::message_t ms1(&FirmwareVersion,sizeof FirmwareVersion, NULL);
  
  //std::cout<<"d0.1 SequenceID = "<<SequenceID<<std::endl;  
  zmq::message_t ms2(&SequenceID,sizeof SequenceID, NULL);
 
  //std::cout<<"d0.2 EventSize = "<<EventSize<<std::endl;
  zmq::message_t ms3(&EventSize,sizeof EventSize, NULL);
 
  //std::cout<<"d0.3 TriggerSize = "<<TriggerSize<<std::endl;
  zmq::message_t ms4(&TriggerSize,sizeof TriggerSize, NULL);
  
  //std::cout<<"d0.4 FIFOOverflow = "<<FIFOOverflow<<std::endl;
  zmq::message_t ms5(&FIFOOverflow,sizeof FIFOOverflow, NULL);
  
//std::cout<<"triggercounts snd size check = "<<triggerCounts.size()<<" card = "<< CardID<<std::endl;
  //std::cout<<"data.size = "<<Data.size()<<std::endl;

  //std::cout<<"d0.5 DriverOverflow = "<<DriverOverflow<<std::endl;
  zmq::message_t ms6(&DriverOverflow, sizeof DriverOverflow, NULL);

  zmq::message_t *tmpms7;
  zmq::message_t *tmpms7b;  
  if(EventSize){
    tmpms7=new zmq::message_t(&(EventIDs[0]), sizeof(uint16_t)*EventIDs.size(), bencleanup2);
    tmpms7b=new zmq::message_t(&(EventTimes[0]), sizeof(uint64_t)*EventTimes.size(), bencleanup2); 
    //std::cout<<"d0.7"<<std::endl;
  }
  //std::cout<<"d0.71 "<<TriggerSize<<std::endl;
  
  zmq::message_t* tmpms8;
  zmq::message_t* tmpms8b;

  //std::cout<<"d0.74"<<std::endl;
if(TriggerSize){
  //std::cout<<"d0.75"<<std::endl;
  tmpms8=new zmq::message_t(&(TriggerMasks[0]), sizeof(uint32_t)*TriggerMasks.size(), bencleanup2);
  tmpms8b=new zmq::message_t(&(TriggerCounters[0]), sizeof(uint32_t)*TriggerCounters.size(), bencleanup2);  
//std::cout<<"d0.12"<<std::endl;
}
 //std::cout<<"d0.76"<<std::endl;
  //  std::cout<<"data.size = "<<Data.size()<<std::endl;
  // std::cout<<"data.size2 = "<<Data.size()*sizeof(uint16_t)<<std::endl;
  //std::cout<<"d1"<<std::endl;  
/*  for (int i=0;i<Data.size();i++){

if(Data.at(i)==0)    std::cout<<" data.at("<<i<<")="<<Data.at(i);
  }
  std::cout<<std::endl;
  */
  //zmq::message_t ms8(&fullbuffsize,sizeof fullbuffsize, bencleanup);

  socket->send(ms1,ZMQ_SNDMORE);
  socket->send(ms2,ZMQ_SNDMORE);
  socket->send(ms3,ZMQ_SNDMORE);
  socket->send(ms4,ZMQ_SNDMORE);
  socket->send(ms5,ZMQ_SNDMORE);
  if(EventSize || TriggerSize) socket->send(ms6,ZMQ_SNDMORE);
  else socket->send(ms6);

  //std::cout<<"testing send events"<<std::endl;
  if (EventSize){
    //std::cout<<"sendng events"<<std::endl;
    socket->send(*tmpms7,ZMQ_SNDMORE);
    if(TriggerSize)socket->send(*tmpms7b,ZMQ_SNDMORE);
    else socket->send(*tmpms7b);
    //std::cout<<"sent"<<std::endl;     
  } 
  //std::cout<<"testing sending data"<<std::endl;
  if(TriggerSize){  
    //std::cout<<"sending data"<<std::endl;
    socket->send(*tmpms8,ZMQ_SNDMORE);
    socket->send(*tmpms8b);
    //std::cout<<"sent"<<std::endl;
  }
  
  
  //std::cout<<"finnished, deleteing. EventSize="<<EventSize<<std::endl;
  if(EventSize){
    // std::cout<<"im deleting"<<std::endl;
    delete tmpms7;
    tmpms7=0;
    delete tmpms7b;
    tmpms7b=0;
  }
  if(TriggerSize){
    delete tmpms8;
    tmpms8=0;
    delete tmpms8b;
    tmpms8b=0;
  }
    //std::cout<<"d2"<<std::endl;
  //std::cout<<" ben 1"<<std::endl;
  //for(int i=0;i<channels;i++){
  //zmq::message_t ms(&PMTID[0],&PMTID[channels]);
  // zmq::message_t ms(PMTID,sizeof(int)*channels,bencleanup);
  //zmq::message_t ms(&PMTID[i],sizeof PMTID[i], bencleanup);
  //  std::cout<<" ben 2"<<EventIDs.size()<<std::endl;

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
  //    zmq::message_t mss(Data, sizeof(uint16_t)*fullbuffsize, bencleanup); 

    //std::cout<<" ben 4"<<std::endl;
    
      //zmq::message_t ms(&Data[i],sizeof Data[i], bencleanup);
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
