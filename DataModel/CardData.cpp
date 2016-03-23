#include "CardData.h"

void bencleanup (void *data, void *hint) {
  free (data);
}

CardData::~CardData(){
  //    std::cout<<" c d 1"<<std::endl;                                               
  delete[] PMTID;
  PMTID=0;
  // std::cout<<" c d 2"<<std::endl;                                                  
  delete[] Data;
  Data=0;
  //std::cout<<" c d 3"<<std::endl;                                                   

}

void  CardData::Send(zmq::socket_t *socket){

  zmq::message_t ms1(&LastSync,sizeof LastSync, bencleanup);
  zmq::message_t ms2(&SequenceID,sizeof SequenceID, bencleanup);
  zmq::message_t ms3(&StartTime,sizeof StartTime, bencleanup);
  zmq::message_t ms4(&CardID,sizeof CardID, bencleanup);
  zmq::message_t ms5(&channels,sizeof channels, bencleanup);
  zmq::message_t ms6(&buffersize,sizeof buffersize, bencleanup);
  zmq::message_t ms7(&fullbuffsize,sizeof fullbuffsize, bencleanup);

  socket->send(ms1,ZMQ_SNDMORE);
  socket->send(ms2,ZMQ_SNDMORE);
  socket->send(ms3,ZMQ_SNDMORE);
  socket->send(ms4,ZMQ_SNDMORE);
  socket->send(ms5,ZMQ_SNDMORE);
  // socket->send(ms6,ZMQ_SNDMORE);                                                   
  socket->send(ms7,ZMQ_SNDMORE);

  for(int i=0;i<channels;i++){
    zmq::message_t ms(&PMTID[i],sizeof PMTID[i], bencleanup);
    socket->send(ms,ZMQ_SNDMORE);
  }

  for(int i=0;i<fullbuffsize;i++){
    zmq::message_t ms(&Data[i],sizeof Data[i], bencleanup);
    socket->send(ms,ZMQ_SNDMORE);
  }

  socket->send(ms6);


}

void CardData::Receive(zmq::socket_t *socket){
  /*
  int more;                                                                   
  size_t more_size = sizeof (more);                                           
  // Receive->getsockopt(ZMQ_RCVMORE, &more, &more_size);                     
                                                                                
  zmq::message_t message;                                                     
  //        std::cout<<"about to get message"<<std::endl;                     
                                                                                
  while (true){                                
    IF(Receive->recv(&message)){   
  */  
  
  zmq::message_t message;

  socket->recv(&message);
  LastSync=*(reinterpret_cast<int*>(message.data()));
  socket->recv(&message);
  SequenceID=*(reinterpret_cast<int*>(message.data()));
  socket->recv(&message);
  StartTime=*(reinterpret_cast<int*>(message.data()));
  socket->recv(&message);
  CardID=*(reinterpret_cast<int*>(message.data()));
  socket->recv(&message);
  channels=*(reinterpret_cast<int*>(message.data()));
  socket->recv(&message);
  fullbuffsize=*(reinterpret_cast<int*>(message.data()));
  
  PMTID=new int[channels];
  socket->recv(&message);
  int* PMTIDtmp=(reinterpret_cast<int*>(message.data()));
  
  std::copy(PMTIDtmp, PMTIDtmp+channels, PMTID);
//for(int i=0;i<channels;i++){
  // PMTID[i]=PMTIDtmp[i];
    //    socket->recv(&message);
    // PMTID[i]=*(reinterpret_cast<int*>(message.data()));
  //}
  
  Data=new uint16_t[fullbuffsize];
  socket->recv(&message);
  // std::vector<uint16_t> Datatmp(*(reinterpret_cast<std::vector<uint16_t>* > (message.data())));


  // std::vector<char> Datatmp(zmq_msg_size(&messageIn));
  //std::memcpy(inMsg.data(), zmq_msg_data(&messageIn), zmq_msg_size(&messageIn));

   uint16_t *Datatmp=(reinterpret_cast<uint16_t* >(message.data()));
  // std::vector<uint16_t> Datatmp (myints, myints + sizeof(myints) / sizeof(uint16_t) );


  //std::cout<<"vector size "<<Datatmp.size()<<std::endl;


   std::copy(Datatmp, Datatmp+fullbuffsize, Data);

   //  for(int i=0;i<fullbuffsize;i++){
   //Data[i]=myints[i];

    //socket->recv(&message);
    //Data[i]=*(reinterpret_cast<uint16_t*>(message.data()));
   //}


  socket->recv(&message);
  buffersize=*(reinterpret_cast<int*>(message.data()));

      /*
      Receive->getsockopt(ZMQ_RCVMORE, &more, &more_size);                    
          
}
                                                        
    if(more==0) break;
      */


}
