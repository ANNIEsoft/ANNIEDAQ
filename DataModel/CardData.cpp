#include "CardData.h"

void bencleanup (void *data, void *hint) {
  //  free (data);
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

  socket->send(ms1);
  socket->send(ms2);
  socket->send(ms3);
  socket->send(ms4);
  socket->send(ms5);
  // socket->send(ms6);                                                   
  socket->send(ms7);


  //std::cout<<" ben 1"<<std::endl;
  //for(int i=0;i<channels;i++){
  //zmq::message_t ms(&PMTID[0],&PMTID[channels]);
  zmq::message_t ms(PMTID,sizeof(int)*channels,bencleanup);
  //zmq::message_t ms(&PMTID[i],sizeof PMTID[i], bencleanup);
    // std::cout<<" ben 2"<<std::endl;

    socket->send(ms);
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
    zmq::message_t mss(Data, sizeof(uint16_t)*fullbuffsize, bencleanup); 

    //std::cout<<" ben 4"<<std::endl;
    
      //zmq::message_t ms(&Data[i],sizeof Data[i], bencleanup);
    socket->send(mss);
    //std::cout<<" ben 5"<<std::endl;

    //}

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
  for(int i=0;i<channels;i++){
    socket->recv(&message);
    PMTID[i]=*(reinterpret_cast<int*>(message.data()));
  }

  Data=new uint16_t[fullbuffsize];
  for(int i=0;i<fullbuffsize;i++){
    socket->recv(&message);
    Data[i]=*(reinterpret_cast<uint16_t*>(message.data()));
  }


  socket->recv(&message);
  buffersize=*(reinterpret_cast<int*>(message.data()));

      /*
      Receive->getsockopt(ZMQ_RCVMORE, &more, &more_size);                    
          
}
                                                        
    if(more==0) break;
      */


}
