#include "DataRecorder.h"

DataRecorder::DataRecorder():Tool(){}


bool DataRecorder::Initialise(std::string configfile, DataModel &data){
  
  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();
  
  m_data= &data;
  
  m_variables.Get("TTreeEventCap",m_TTreeEventCap); 
  m_variables.Get("TFileTTreeCap",m_TFileTTreeCap);
  m_variables.Get("OutputPath",OutputPath);

  m_filepart=0;  
  std::stringstream tmp;
  tmp<<OutputPath<<"DataR"<<m_data->RunNumber<<"S"<<m_data->SubRunNumber;
  OutFile=tmp.str();
  tmp<<"p0.root";
  TFile file(tmp.str().c_str(),"RECREATE","",0);
  file.Write();
  file.Close();
  
  //std::cout<< "outfile = "<<OutFile<<std::endl;
  
  Isend = new zmq::socket_t(*(m_data->context), ZMQ_PUSH);
  Isend->bind("inproc://RootWriter");
  
  args=new root_thread_args(OutFile,m_data->context,m_TFileTTreeCap, m_filepart);
  //  //pthread_create (&thread[0], NULL, DataRecorder::FlowManager, args);
  // pthread_create (&thread[0], NULL, DataRecorder::RootWriter, args);
  
  h1 = new TThread("h1", DataRecorder::RootWriter, args);
  h1->Run();
  
  m_treenum=0;
  m_data->NumEvents=0;
  
  PMTID=new int[m_data->channels.at(0)];
  Data=new double[m_data->fullbuffsize.at(0)];
  
  TTree *tree = new TTree("PMTData0","PMTData0");
  
  tree->Branch("LastSync",&LastSync,"LastSync/I");
  tree->Branch("SequenceID",&SequenceID,"SequenceID/I");
  tree->Branch("StartTime",&StartTime,"StartTime/I");
  tree->Branch("CardID",&CardID,"CardID/I");
  tree->Branch("Channels",&channels,"Channels/I");
  tree->Branch("PMTID",PMTID,"PMTID[Channels]/I");
  tree->Branch("BufferSize",&buffersize,"BufferSize/I");
  tree->Branch("FullBufferSize",&fullbuffsize,"FullBufferSize/I");
  tree->Branch("Data",Data,"Data[FullBufferSize]/D");
  
  m_data->AddTTree("PMTData",tree);
  
  return true;
}


bool DataRecorder::Execute(){
  //std::cout<<"Debug 1 i="<<m_data->NumEvents<<std::endl;
  m_data->NumEvents++;
  
  TTree *tree=m_data->GetTTree("PMTData");
  //std::cout<<"Debug 2"<<std::endl;
  if (tree->GetEntriesFast()>m_TTreeEventCap){
    //std::cout<<"Debug 3"<<std::endl;
    //    root_thread_args *args=new root_thread_args(OutFile,tree);
    //std::cout<<"Debug 4 "<<tree<<std::endl;
    
    
    zmq::message_t message(512);
    std::stringstream TTreepointer;
    TTreepointer<<"TTree "<<tree;
    
    //std::cout<<"sending "<<TTreepointer.str()<<std::endl;
    
    snprintf ((char *) message.data(), 512 , "%s" ,TTreepointer.str().c_str()) ;
    Isend->send(message);
    
    //std::cout<<"sent "<<std::endl;
    //    pthread_create (&thread[0], NULL, DataRecorder::WriteOut, args);
    //std::cout<<"Debug 5"<<std::endl;
    m_treenum++;
    std::stringstream tmp;
    tmp<<"PMTData"<<m_treenum;
    //std::cout<<"Debug 6 m_tmp = "<<tmp.str()<<std::endl;
    tree=new TTree(tmp.str().c_str(),tmp.str().c_str());
    //std::cout<<"Debug 7"<<std::endl;
    m_data->AddTTree("PMTData",tree);
    //std::cout<<"Debug 8"<<std::endl;
    tree->Branch("LastSync",&LastSync,"LastSync/I");
    tree->Branch("SequenceID",&SequenceID,"SequenceID/I");
    tree->Branch("StartTime",&StartTime,"StartTime/I");
    tree->Branch("CardID",&CardID,"CardID/I");
    tree->Branch("Channels",&channels,"Channels/I");
    tree->Branch("PMTID",PMTID,"PMTID[Channels]/I");
    tree->Branch("BufferSize",&buffersize,"BufferSize/I");
    tree->Branch("FullBufferSize",&fullbuffsize,"FullBufferSize/I");
    tree->Branch("Data",Data,"Data[FullBufferSize]/D");
    //std::cout<<"Debug 9"<<std::endl;
  }
  //std::cout<<"Debug 10"<<std::endl;
  for(int i=0;i<m_data->LastSync.size();i++){
    //std::cout<<"Debug 11"<<std::endl;
    LastSync=m_data->LastSync.at(i);
    SequenceID=m_data->SequenceID.at(i);
    StartTime=m_data->StartTime.at(i);
    CardID=m_data-> CardID.at(i);
    channels=m_data->channels.at(i);
    buffersize=m_data->buffersize.at(i);
    fullbuffsize=m_data->fullbuffsize.at(i);
     //std::cout<<"Debug 12"<<std::endl;
    for(int j=0;j<channels;j++){
      //std::cout<<"Debug 13"<<std::endl;

      PMTID[j]=m_data->PMTID.at(i)[j]; // these pointers have changed hence the crash
      //std::cout<<"Debug 14"<<std::endl;
      
    }	
    //std::cout<<"Debug 15"<<std::endl;
    
    for(int j=0;j<fullbuffsize;j++){
      //std::cout<<"Debug 16"<<std::endl;
      
      Data[j]=m_data->Data.at(i)[j];
      //std::cout<<"Debug 17"<<std::endl;

    }
    
    //std::cout<<"Debug 18 i= "<<m_data->NumEvents<<std::endl;
     tree->Fill();
    //std::cout<<"Debug 19"<<std::endl;
    
  }
  //std::cout<<"Debug 20"<<std::endl;
  
  
  
  
  return true;
}


bool DataRecorder::Finalise(){

 

  zmq::message_t message(256);
  std::string send="Quit 0x00";

  //std::cout<<"sending "<<send<<std::endl;

  snprintf ((char *) message.data(), 256 , "%s" ,send.c_str()) ;
  Isend->send(message);

  //TThread::Ps();
  h1->Join();
  //  (void) pthread_join(thread[0], NULL);
  m_filepart++;  
  std::stringstream tmp;
  tmp<<OutFile<<"p"<<m_filepart<<".root";
  TFile file(tmp.str().c_str(),"UPDATE","",0);
  
  TTree *tree=m_data->GetTTree("PMTData");
  tree->Write();
  file.Write();
  file.Close();
  
  // sleep(5);

  m_data->DeleteTTree("PMTData");
  tree=0;

  //  Isend->close();

  // too delete properly 

  delete h1;
  h1=0;

  delete Isend;
  Isend=0;
  
  delete args;
  args=0;

  delete PMTID;
  PMTID=0;

  delete Data;
  Data=0;
  
  return true;
}

/*
void* DataRecorder::WriteOut(void* arg){
  //  std::cout<<"Debug a1"<<std::endl;
  root_thread_args* args= static_cast<root_thread_args*>(arg);
  //std::cout<<"Debug a2"<<std::endl;

  std::cout<<args->OutFile<<std::endl;
  //  std::cout<<"Debug a3"<<std::endl;

  TFile file(args->OutFile.c_str(),"UPDATE");
  //  std::cout<<"Debug a4 "<<args->tree<<std::endl;

  args->tree->Write();
  //std::cout<<"Debug a5"<<std::endl;

  file.Write();
  //std::cout<<"Debug a6"<<std::endl;

  file.Close();
  //std::cout<<"Debug a7"<<std::endl;

  delete args->tree;
  //std::cout<<"Debug a8"<<std::endl;

  args->tree=0;
  std::cout<<"Debug a9"<<std::endl;
  
  return (NULL);
  
}

*/


void* DataRecorder::RootWriter(void* arg){
  
  root_thread_args* args= static_cast<root_thread_args*>(arg);
 
  zmq::socket_t Ireceive (*(args->context), ZMQ_PULL);
  Ireceive.connect("inproc://RootWriter");

  bool running=true;
  *(args->filepart)=0;
  int treenum=0;
   

  while (running){
    //std::cout<<"T  Debug 1"<<std::endl;
    zmq::message_t comm;
    //std::cout<<"T  Debug 2"<<std::endl;
    Ireceive.recv(&comm);
    
    std::istringstream iss(static_cast<char*>(comm.data()));
    std::string arg1="";
    long long unsigned int arg2;    
    //lon arg2="";
    TTree *tree;
    
    iss>>arg1>>std::hex>>arg2;
    
    //std::cout<<"arg1 = "<<arg1<<std::endl;
    //std::cout<<"arg2 = "<<arg2<<std::endl;
    //std::cout<<"T  Debug 3"<<std::endl;
    if (arg1=="TTree"){
      //std::cout<<"T  Debug 4"<<std::endl;
      treenum++;
     
      if(treenum>args->TFileTTreeCap){
	(*(args->filepart))+=1;
	treenum=1;	
      }
      tree=reinterpret_cast<TTree *>(arg2);
      std::stringstream tmp;
      tmp<<args->OutFile<<"p"<<*(args->filepart)<<".root";
      TFile file(tmp.str().c_str(),"UPDATE","",0);
      //std::cout<<"tree = "<<tree<<std::endl;
      tree->Write();
      //std::cout<<"T  Debug 5"<<std::endl;
      file.Write();
      //std::cout<<"T Debug 6"<<std::endl;
      file.Close();
      
      //std::cout<<"T  Debug 7"<<std::endl;
      //sleep(5);
      //std::cout<<"T  Debug 8"<<std::endl;
      delete tree;
      //std::cout<<"T  Debug 9"<<std::endl;
      tree=0;
      //std::cout<<"T  Debug 10"<<std::endl;
     
    }
    
    else if(arg1=="Quit"){
      running=false;
      //std::cout<<"T  Debug 12"<<std::endl;
    }
    //std::cout<<"T  Debug 13"<<std::endl;
  }
  //std::cout<<"T  Debug 14"<<std::endl;

  //std::cout<<"exiting thread"<<std::endl;
  //pthread_exit(NULL);
  //std::cout<<"exit"<<std::endl;
}
