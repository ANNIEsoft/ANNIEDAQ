#include "TriggerData.h"

TriggerData::TriggerData(){

  Init();

}

void TriggerData::Init(){
  
  //std::cout<<"b0"<<std::endl;
 
  tree = new TTree("TrigData","TrigData");
  //    EventIDs=new short[500];
  //EventTimes=new long[500];
  //TriggerMasks=new uint32_t[500];
  //TriggerCounters= new uint32_t[500];
  //std::cout<<"b1"<<std::endl;
  TBranch *br;
  //std::cout<<"b2"<<std::endl;
  br=tree->Branch("FirmwareVersion",&(FirmwareVersion),"FirmwareVersion/I");
  //std::cout<<"b3"<<std::endl;
  br->SetCompressionLevel(1);
  //std::cout<<"b4"<<std::endl;
  br=tree->Branch("SequenceID",&(SequenceID),"SequenceID/I");
  //std::cout<<"b5"<<std::endl;
  br->SetCompressionLevel(1);
  //std::cout<<"b6"<<std::endl;
  br=tree->Branch("EventSize",&(EventSize),"EventSize/I");
  //std::cout<<"b7"<<std::endl;
  br->SetCompressionLevel(1);
  //std::cout<<"b8"<<std::endl;
  br=tree->Branch("TriggerSize",&(TriggerSize),"TriggerSize/I");
  //std::cout<<"b9"<<std::endl;
  br->SetCompressionLevel(1);
  //std::cout<<"b10"<<std::endl;
  br=tree->Branch("FIFOOverflow",&(FIFOOverflow),"FIFOOverflow/I");
  //std::cout<<"b11"<<std::endl;
  br->SetCompressionLevel(1);
  //std::cout<<"b12"<<std::endl;
  br=tree->Branch("DriverOverfow",&(DriverOverflow),"DriverOverflow/I");
  //std::cout<<"b13"<<std::endl;
  br->SetCompressionLevel(1);
  //std::cout<<"b14"<<std::endl;
  //br=tree->Branch("EventIDs",&EventIDs[0],"EventIDs[EventSize]/s");
  br=tree->Branch("EventIDs",EventIDs,"EventIDs[EventSize]/s");
  br->SetCompressionLevel(1);
  //std::cout<<"b15"<<std::endl;
  br=tree->Branch("EventTimes",EventTimes, "EventTimes[EventSize]/l");
  //    br=tree->Branch("EventTimes",&EventTimes[0], "EventTimes[EventSize]/l");  
  br->SetCompressionLevel(1);
  //std::cout<<"b16"<<std::endl;
  // br=tree->Branch("TriggerMasks",&TriggerMasks);
  br=tree->Branch("TriggerMasks",TriggerMasks,"TriggerMasks[TriggerSize]/i");    
  br->SetCompressionLevel(1);    
  //br=tree->Branch("TriggerCounters",&TriggerCounters);   
  br=tree->Branch("TriggerCounters",TriggerCounters,"TriggerCounters[TriggerSize]/i");
  br->SetCompressionLevel(1);
  
}

void  TriggerData::Send(zmq::socket_t *socket){
  
  //  std::cout<<"d0"<<std::endl;
  zmq::message_t ms1(&FirmwareVersion,sizeof FirmwareVersion, NULL);
  // std::cout<<"d0.1"<<std::endl;  
zmq::message_t ms2(&SequenceID,sizeof SequenceID, NULL);
//std::cout<<"d0.2"<<std::endl;

//  zmq::message_t ms3(&eventsize,sizeof eventsize, NULL);
  //std::cout<<"d0.3"<<std::endl;

  //zmq::message_t ms4(&datasize,sizeof datasize, NULL);
  //std::cout<<"d0.4"<<std::endl;

  zmq::message_t ms5(&FIFOOverflow,sizeof FIFOOverflow, NULL);
  //std::cout<<"triggercounts snd size check = "<<triggerCounts.size()<<" card = "<< CardID<<std::endl;
  //std::cout<<"data.size = "<<Data.size()<<std::endl;
  //std::cout<<"d0.5"<<std::endl;

  zmq::message_t ms6(&DriverOverflow, sizeof DriverOverflow, NULL);
  //std::cout<<"d0.6"<<std::endl;
  /////////////////////////////////////////////////
  /*
  zmq::message_t ms7(&(Events.at(0)), sizeof(TriggerDriverEventLog)*Events.size(), NULL); 
  //std::cout<<"d0.7"<<std::endl;

  zmq::message_t ms8(&(Data.at(0)), sizeof(TriggerTimeStamp)*Data.size(), NULL);
  //  std::cout<<"d0.12"<<std::endl;

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
  //  socket->send(ms3,ZMQ_SNDMORE);
  //socket->send(ms4,ZMQ_SNDMORE);
  //socket->send(ms5,ZMQ_SNDMORE);
  ///  socket->send(ms6,ZMQ_SNDMORE);                              
  //socket->send(ms7,ZMQ_SNDMORE);
  // socket->send(ms8);

   // std::cout<<"d2"<<std::endl;
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

bool TriggerData::Receive(zmq::socket_t *socket){
  /*
  int more;                                                                   
  size_t more_size = sizeof (more);                                           
  // Receive->getsockopt(ZMQ_RCVMORE, &more, &more_size);                     
                                                                                
  zmq::message_t message;                                                     
  //        std::cout<<"about to get message"<<std::endl;                     
                                                                                
  while (true){                                
    IF(Receive->recv(&message)){   
  */  
  //std::cout<<"about to receive trig data"<<std::endl;
  zmq::message_t message;
  //std::cout<<"d0"<<std::endl;
  if(socket->recv(&message)){
    FirmwareVersion=*(reinterpret_cast<int*>(message.data()));
    //std::cout<<"d1 FirmwareVersion = "<<FirmwareVersion<<std::endl;    

    if(socket->recv(&message)){
      SequenceID=*(reinterpret_cast<int*>(message.data()));
      //std::cout<<"d2 SequenceID = "<<SequenceID<<std::endl;

      if(socket->recv(&message)){
	EventSize=*(reinterpret_cast<int*>(message.data()));
	//std::cout<<"d3 EventSize = "<<EventSize<<std::endl;

	if(socket->recv(&message)){
	  TriggerSize=*(reinterpret_cast<int*>(message.data()));
	  //std::cout<<"d4 TriggerSize = "<<TriggerSize<<std::endl;

	  if(socket->recv(&message)){
	    FIFOOverflow=*(reinterpret_cast<int*>(message.data()));
	    //std::cout<<"d5 FIFOOverflow = "<<FIFOOverflow<<std::endl;

	    
	    if(socket->recv(&message)){
	      DriverOverflow=*(reinterpret_cast<int*>(message.data()));
	      //std::cout<<"d6 DriverOverflow = "<<DriverOverflow<<std::endl;
	      
	      if(EventSize){
		if(socket->recv(&message)){
		  //std::cout<<"d7"<<std::endl;
		  //		  EventIDs.resize(message.size()/(sizeof(uint16_t)));
		  //std::cout<<"d8"<<std::endl;
		  //short tmp[EventSize];
		   std::memcpy(EventIDs, message.data(), message.size());
		   //   for(int i=0;i<EventSize;i++){
		   // EventIDs[i]=tmp[i];
		   // }
		  //std::cout<<"d9"<<std::endl;
		}
		else return false;
		
		if(socket->recv(&message)){
		  //std::cout<<"d7"<<std::endl;
		  
                  //EventTimes.resize(message.size()/(sizeof(uint64_t)));
		  //std::cout<<"d8"<<std::endl;

		  //std::memcpy(&EventTimes[0], message.data(), message.size());
		  //long tmp[EventSize];
		  	  std::memcpy(EventTimes, message.data(), message.size());
		  
			  //for(int i=0;i<EventSize;i++){
		    //EventTimes[i]=tmp[i];
			  //}		  
		  //std::cout<<"d9"<<std::endl;
		  
		}
		else return false;
	      }
	      
	      //std::cout<<"d9.5"<<std::endl;
	      
	      if(TriggerSize){
		zmq::message_t message;
		if(socket->recv(&message)){
		  //TriggerMasks.resize(message.size()/(sizeof(uint32_t)));
		  //	  int tmp[TriggerSize];
		  //std::memcpy(&TriggerMasks[0], message.data(), message.size());
		  //int *tmp;
		  // tmp=reinterpret_cast<int*>(message2.data());
		  std::memcpy(TriggerMasks, message.data(), message.size());
		  //for(int i=0;i<TriggerSize;i++){
		  // std::cout<<"ts "<<i<<std::endl;
		  //  TriggerMasks[i]=1;
		  // }

		}
		else return false;
		
		if(socket->recv(&message)){
                  //TriggerCounters.resize(message.size()/(sizeof(uint32_t)));
		  //std::memcpy(&TriggerCounters[0], message.data(), message.size());		  
		  //		  int tmp[TriggerSize];
		  std::memcpy(TriggerCounters, message.data(), message.size());
		  //for(int i=0;i<TriggerSize;i++){
		    // TriggerCounters[i]=tmp[i];
		  // }

		}
		else return false;
	      }
	      
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
  
  
  //std::cout<<"d10"<<std::endl;
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
  return true;

}
