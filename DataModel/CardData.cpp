#include "CardData.h"

void bencleanup (void *data, void *hint) {
  //  free (data);
}

CardData::~CardData(){
  //    std::cout<<" c d 1"<<std::endl;                                               
  //delete[] PMTID;
  //PMTID=0;
  // std::cout<<" c d 2"<<std::endl;                                                  
  delete[] Data;
  Data=0;
  
  delete[] triggerCounts;
  triggerCounts=0;

  delete[] Rates;
  Rates=0;
//std::cout<<" c d 3"<<std::endl;                                                   

}

void  CardData::Send(zmq::socket_t *socket){

  zmq::message_t ms1(&LastSync,sizeof LastSync, bencleanup);
  zmq::message_t ms2(&SequenceID,sizeof SequenceID, bencleanup);
  zmq::message_t ms3(&StartTimeSec,sizeof StartTimeSec, bencleanup);
  zmq::message_t ms4(&StartTimeNSec,sizeof StartTimeNSec, bencleanup);
  zmq::message_t ms5(&StartCount,sizeof StartCount, bencleanup);
  // std::cout<<"triggercounts snd size check = "<<triggerCounts.size()<<std::endl;
  //  zmq::message_t ms6(&(triggerCounts.at(0)), sizeof(uint64_t)*triggerCounts.size(), bencleanup);
  // zmq::message_t ms7(&(Rates.at(0)), sizeof(uint32_t)*Rates.size(), bencleanup); 
  zmq::message_t ms8(&CardID,sizeof CardID, bencleanup);
  zmq::message_t ms9(&channels,sizeof channels, bencleanup);
  zmq::message_t ms10(&buffersize,sizeof buffersize, bencleanup);
  zmq::message_t ms11(&eventsize,sizeof eventsize, bencleanup);
  //zmq::message_t ms12(&(Data.at(0)), sizeof(uint16_t)*Data.size(), bencleanup);
  //zmq::message_t ms8(&fullbuffsize,sizeof fullbuffsize, bencleanup);

  socket->send(ms1,ZMQ_SNDMORE);
  socket->send(ms2,ZMQ_SNDMORE);
  socket->send(ms3,ZMQ_SNDMORE);
  socket->send(ms4,ZMQ_SNDMORE);
  socket->send(ms5,ZMQ_SNDMORE);
  //socket->send(ms6,ZMQ_SNDMORE);                              
  //socket->send(ms7,ZMQ_SNDMORE);
  socket->send(ms8,ZMQ_SNDMORE);
  socket->send(ms9,ZMQ_SNDMORE);
  socket->send(ms10,ZMQ_SNDMORE);
  socket->send(ms11,ZMQ_SNDMORE);
  //socket->send(ms12);
  //std::cout<<" ben 1"<<std::endl;
  //for(int i=0;i<channels;i++){
  //zmq::message_t ms(&PMTID[0],&PMTID[channels]);
  // zmq::message_t ms(PMTID,sizeof(int)*channels,bencleanup);
  //zmq::message_t ms(&PMTID[i],sizeof PMTID[i], bencleanup);
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
  //    zmq::message_t mss(Data, sizeof(uint16_t)*fullbuffsize, bencleanup); 

    //std::cout<<" ben 4"<<std::endl;
    
      //zmq::message_t ms(&Data[i],sizeof Data[i], bencleanup);
  // socket->send(mss);
    //std::cout<<" ben 5"<<std::endl;

    //}

  //socket->send(ms6);


}

bool CardData::Receive(zmq::socket_t *socket){
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
  //  std::cout<<"d1"<<std::endl;
  if(socket->recv(&message)){
    // std::cout<<"d2"<<std::endl;
    LastSync=*(reinterpret_cast<uint64_t*>(message.data()));
    if(socket->recv(&message)){
      //std::cout<<"d3"<<std::endl;

      SequenceID=*(reinterpret_cast<int*>(message.data()));
      if(socket->recv(&message)){
	//std::cout<<"d4"<<std::endl;

	StartTimeSec=*(reinterpret_cast<int*>(message.data()));
	if(socket->recv(&message)){ 
	  //std::cout<<"d5"<<std::endl;

	  StartTimeNSec=*(reinterpret_cast<int*>(message.data()));
	  if(socket->recv(&message)){
	    //std::cout<<"d6"<<std::endl;

	    StartCount=*(reinterpret_cast<uint64_t*>(message.data()));

	    if(socket->recv(&message)){
	      //std::cout<<"d7"<<std::endl;

	      //std::cout<<"triggercounts rec size check = "<<message.size()/(sizeof(long))<<std::endl;
	      triggerCounts=new uint64_t[message.size()/(sizeof(long))];
	      //triggerCounts.resize(message.size()/(sizeof(long)));
	      triggerNumber=message.size()/(sizeof(long));
	      std::memcpy(&triggerCounts[0], message.data(), message.size());
	      
	      if(socket->recv(&message)){
		//std::cout<<"d8"<<std::endl;

		Rates=new uint32_t[message.size()/(sizeof(uint32_t))];
		// Rates.resize(message.size()/(sizeof(int)));
		std::memcpy(&Rates[0], message.data(), message.size());
		
		if(socket->recv(&message)){
		  //std::cout<<"d9"<<std::endl;

		  CardID=*(reinterpret_cast<int*>(message.data()));
		  
		  //std::cout<<"CardID = "<<CardID<<std::endl;
		  if(socket->recv(&message)){
		    //std::cout<<"d10"<<std::endl;

		    channels=*(reinterpret_cast<int*>(message.data()));
		    if(socket->recv(&message)){
		      //std::cout<<"d11"<<std::endl;

		      buffersize=*(reinterpret_cast<int*>(message.data()));
		      buffersize*=4; 
		      //std::cout<<"buffersize = "<<buffersize<<std::endl;
		      if(socket->recv(&message)){
			//std::cout<<"d12"<<std::endl;

			eventsize=*(reinterpret_cast<int*>(message.data()));
			if(socket->recv(&message)){
			  //std::cout<<"d13"<<std::endl;

			  Data=new uint16_t[message.size()/(sizeof(uint16_t))];
			  //std::cout<<"data.size = "<<message.size()<<std::endl;
			  //std::cout<<"data.size2fb = "<<message.size()/(sizeof(uint16_t))<<std::endl;
			  //  Data.resize(message.size()/(sizeof(short)));
			  fullbuffsize=message.size()/(sizeof(uint16_t));
			  //std::cout<<"fb = "<<fullbuffsize<<std::endl;
			  std::memcpy(&Data[0], message.data(), message.size());
			  
			  /*
			    for(int i=0;i<fullbuffsize;i++){
			    
			    if(Data[i]==0)std::cout<<" data.at("<<i<<")="<<Data[i];
			    }
			    std::cout<<" data.at("<<85098<<")="<<Data[85098]<<std::endl;
			  */    
			  /*
			    Receive->getsockopt(ZMQ_RCVMORE, &more, &more_size);                    
			    
			    }
			    
			    if(more==0) break;
			  */
			  // std::cout<<"d14"<<std::endl;

			}
			else return false;			    
		      }
		      else return false;
		    }
		    else return false;
		  }
		  else return false;
		}
		else return false;
	      }
	      else return false;
	    }
	    else return false;
	  }
	  else return false;
	}
	else return false;
      }
      else return false;
    }
    else return false;
  }
  else return false;
 
  return true; 
}
