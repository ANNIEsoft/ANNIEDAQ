#include "VMESendData.h"

VMESendData::VMESendData():Tool(){}


bool VMESendData::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_variables.Get("port",m_port);
  
  Send=new zmq::socket_t(*(m_data->context), ZMQ_PUSH);
  int a=12000;
  Send->setsockopt(ZMQ_SNDTIMEO, a);
  Send->setsockopt(ZMQ_RCVTIMEO, a);

  std::stringstream tmp;
  tmp<<"tcp://*:"<<m_port;
  
  Send->bind(tmp.str().c_str());
  
  return true;
}


bool VMESendData::Execute(){
 
  if(m_data->triggered){
    //std::cout<<"d 1 "<<std::endl; 
    // zmq::message_t Esend(512);
    //snprintf ((char *) Esend.data(), 256 , "%s" ,command.c_str()) ;
    //  StatusCheck.send(Esend);
    
    // create and open a character archive for output
    //std::ofstream ofs("testser");
    
    // std::stringstream archive_stream(std::ios_base::binary| std::ios_base::out| std::ios_base::in);
    //std::stream archive_stream("/dev/null");
    //    std::ostringstream archive_stream;
    // save data to archive
    //std::cout<<"d 2 "<<std::endl;
    //  boost::archive::text_oarchive  oa(archive_stream);
    //    std::cout<<"d 3 "<<m_data->carddata.size()<<std::endl;  
    // write class instance to archive



    //   NEWNEW
    zmq::message_t cards(sizeof m_data->carddata.size());
    snprintf ((char *) cards.data(),  sizeof m_data->carddata.size() , "%d" ,m_data->carddata.size()) ;
    Send->send(cards);
    
    
    for (int i=0;i<m_data->carddata.size();i++){

      
    //NEW ***************************************	  
    //Send->send(m_data->carddata.begin(),m_data->carddata.end());

    
      m_data->carddata.at(i)->Send(Send);
    //std::cout<<" test "<<(m_data->carddata.at(0)->LastSync)<<std::hex<<&(m_data->carddata.at(0)->LastSync)<< std::endl;
    //zmq::message_t ms1(&(m_data->carddata.at(0)->LastSync),sizeof m_data->carddata.at(0)->LastSync, my_free);
  




    //  if(i<m_data->carddata.size()-1)  Send->send(ms1,ZMQ_SNDMORE); 
    //Send->send(ms1); 
   
    /*
    zmq_msg_t msg1;
    zmq_msg_init_data (&msg1, matrix1, matrix1_size, my_free, NULL);
    zmq_send (s, &msg, ZMQ_SNDMORE);
    zmq_msg_t msg2;
    zmq_msg_init_data (&msg2, matrix2, matrix2_size, my_free, NULL);
    zmq_send (s, &msg, 0);
    */
		  
    //*********************************************
      /*
      //std::cout<<"d 4 "<<std::endl;
      std::ostringstream archive_stream;
      //  std::iostream archive_stream;
      boost::archive::binary_oarchive  oa(archive_stream);
      
      oa << *(m_data->carddata.at(i));
      


      //    m_data->Crate->EnableTrigger();
      // archive and stream closed when destructors are called
      
      //std::cout<<"d 5 "<<std::endl;
      //  std::cout<<"run number = "<<m_data->RunNumber<<std::endl;
      //std::cout<<"sub run number = "<<m_data->SubRunNumber<<std::endl;
      //std::cout<<"data 3 = "<<m_data->carddata.at(0)->Data[3]<<std::endl;
      // ... some time later restore the class instance to its orginal state
      /* gps_position newg;
	 
      {
       // create and open an archive for input
       std::ifstream ifs("filename");
       boost::archive::text_iarchive ia(ifs);
       // read class state from archive
       ia >> newg;
       // archive and stream closed when destructors are called
       }
      */

      /*

      //std::cout<<"d 6 "<<std::endl;
     
      //send(sock, &vect[0], vect.size());
      std::string tmp=archive_stream.str();
	std::vector<char> bytes(tmp.begin(), tmp.end());
      //std::cout<<tmp<<std::endl;
	std::cout<<"d 7 "<<tmp.length()+1<<std::endl;
      
	int s=(tmp.length());
	zmq::message_t size(sizeof s);
	memcpy ((void *) size.data(),&s , sizeof s);   
	Send->send(size);

	zmq::message_t message(tmp.begin(), tmp.end());
	//zmq::message_t message(tmp.length()+1);
	  //  memcpy ((void *) message.data(),&bytes , bytes.size());
	//memcpy ((void *) message.data(),&tmp , tmp.length()+1);
      
      std::cout<<"d 8 "<<bytes.at(500)<<" = "<< (reinterpret_cast<char*>(message.data()))[500] <<" : "<< message.size()<<std::endl; 
      
           char* test=(reinterpret_cast<char*>(message.data()));
	   for (int j=0;j<bytes.size();j++){

	     //	     std::cout<<test[i];

	     if(j<10) std::cout<<test[j];
	     if(j>bytes.size()-10) std::cout<<test[j];

	   }
	   std::cout<<std::endl<<bytes.size()<<std::endl;
	   std::cout<<message.size()<<std::endl;
      
	   //      Send->send(message);

      std::stringstream test2;
      if(i==0){    
	std::ofstream myfile;
	myfile.open ("example.txt");    
	for(int i=0;i<message.size();i++){
	  test2<<test[i];
	  myfile<<test[i];
	}
	
	myfile.close();
      }

      std::cout<<"String test2 = "<< test2.str().length()<<" string tmp = "<<tmp.length()<<std::endl;
      std::cout<<" test "<<(test2.str()==tmp)<<std::endl;
      
      //std::stringstream test2(tmp);
      Send->send(message);

      boost::archive::binary_iarchive ia(test2);

      /*
      zmq::message_t message(archive_stream.tellp());
      memcpy ((void *) message.data(), archive_stream.rdbuf(), archive_stream.tellp());
      //snprintf ((char *) message.data(),  tmp.length()+1 , "%s" ,tmp.c_str()) ;
      std::cout<<"d 8 "<<*(reinterpret_cast<std::string*>(message.data()))<<std::endl;
      //if(i<m_data->carddata.size()-1)  Send->send(message,ZMQ_SNDMORE);
      //else
      Send->send(message);
      //std::cout<<"d 9 "<<std::endl;*/
    }
    
    
  }
  
  return true;
}


bool VMESendData::Finalise(){
  
  delete  Send;
  Send=0;

  return true;
}


