#include "RootDataRecorder.h"

RootDataRecorder::RootDataRecorder():Tool(){}


bool RootDataRecorder::Initialise(std::string configfile, DataModel &data){
  
  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();
  
  m_data= &data;
  
  m_variables.Get("TTreeEventCap",m_TTreeEventCap); 
  m_variables.Get("TFileTTreeCap",m_TFileTTreeCap);
  m_variables.Get("OutputPath",OutputPath);
  m_variables.Get("MRDEnable",MRDEnable);

  m_filepart=0;  
  std::stringstream tmp;
  tmp<<"DataR"<<m_data->RunNumber<<"S"<<m_data->SubRunNumber;
  std::string filename=tmp.str();
  tmp.str("");
  tmp<<OutputPath<<"DataR"<<m_data->RunNumber<<"S"<<m_data->SubRunNumber;
  OutFile=tmp.str();
  tmp<<"p0.root";
  TFile file(tmp.str().c_str(),"RECREATE","",1);
  file.Write();
  file.Close();
  
  //std::cout<< "outfile = "<<OutFile<<std::endl;
  
  Isend = new zmq::socket_t(*(m_data->context), ZMQ_PUSH);
  //std::cout<<"attempting bind"<<std::endl;
  Isend->bind("inproc://RootWriter");
  //std::cout<<"bound"<<std::endl;
  
  args=new rootw_thread_args(OutFile, filename, m_data->context,m_TFileTTreeCap, m_filepart,m_data->GetTTree("RunInformation"),MRDEnable);
  //  //pthread_create (&thread[0], NULL, RootDataRecorder::FlowManager, args);
  // pthread_create (&thread[0], NULL, RootDataRecorder::RootWriter, args);
  
  h1 = new TThread("h1", RootDataRecorder::RootWriter, args);
  h1->Run();
  
  m_treenum=0;
  m_data->NumEvents=0;
  //std::cout<<"i d1 "<<std::endl;
  // std::cout<<m_data->carddata.at(0)->channels<<std::endl;

  //  m_card.PMTID=new int[4];
  //std::cout<<"i d2"<<std::endl;  
  m_card.Data=new uint16_t[160000];
  m_card.Rates=new uint32_t[160000];
  m_card.triggerCounts=new uint64_t[160000];
  //std::cout<<"i d3"<<std::endl;
  TTree *tree = new TTree("PMTData","PMTData");
  TBranch *br;
  //std::cout<<"i d4"<<std::endl;
  br=tree->Branch("LastSync",&(m_card.LastSync),"LastSync/l");
  br->SetCompressionLevel(1);
  br=tree->Branch("SequenceID",&(m_card.SequenceID),"SequenceID/I");
  br->SetCompressionLevel(1);  
  br=tree->Branch("StartTimeSec",&(m_card.StartTimeSec),"StartTimeSec/I");
  br->SetCompressionLevel(1);  
  br=tree->Branch("StartTimeNSec",&(m_card.StartTimeNSec),"StartTimeNSec/I");
  br->SetCompressionLevel(1);
  br=tree->Branch("StartCount",&(m_card.StartCount),"StartCount/l");
  br->SetCompressionLevel(1);  
  br=tree->Branch("TriggerNumber",&(m_card.triggerNumber),"TriggerNumber/I");
  br->SetCompressionLevel(1);  
  br=tree->Branch("TriggerCounts",m_card.triggerCounts,"TriggerCounts[TriggerNumber]/l");
  br->SetCompressionLevel(1);  
  //  tree->Branch("TriggerCounts",&(m_card.triggerCounts)); 
  //tree->Branch("Rates",&(m_card.Rates)); 
  br=tree->Branch("CardID",&(m_card.CardID),"CardID/I");
  br->SetCompressionLevel(1);  
  br=tree->Branch("Channels",&(m_card.channels),"Channels/I");
  br->SetCompressionLevel(1);  
  br=tree->Branch("Rates",m_card.Rates,"Rates[Channels]/i");
  br->SetCompressionLevel(1); 
  //tree->Branch("PMTID",m_card.PMTID,"PMTID[Channels]/I");
  br=tree->Branch("BufferSize",&(m_card.buffersize),"BufferSize/I");
  br->SetCompressionLevel(1);  
  br=tree->Branch("Eventsize",&(m_card.eventsize),"Eventsize/I");
  br->SetCompressionLevel(1);  
  br=tree->Branch("FullBufferSize",&(m_card.fullbuffsize),"FullBufferSize/I");
  br->SetCompressionLevel(1); 
  //tree->Branch("Data",&(m_card.Data));
  br=tree->Branch("Data",m_card.Data,"Data[FullBufferSize]/s");
  br->SetCompressionLevel(1);
  //std::cout<<"i d5"<<std::endl;
  m_data->AddTTree("PMTData",tree);
  //std::cout<<"i d6"<<std::endl;
  
  m_data->InfoTitle="RootDataRecorderVariables";
  m_variables>>m_data->InfoMessage;
  m_data->GetTTree("RunInformation")->Fill();


  return true;
}


bool RootDataRecorder::Execute(){
  if(m_data->Restart==1)Finalise();
  else if(m_data->Restart==2)Initialise("",*m_data);
  else{
  //std::cout<<"Debug 1 i="<<m_data->NumEvents<<std::endl;
  if(m_data->triggered){

    //    boost::progress_timer t;

  m_data->NumEvents++;
  std::stringstream eventstatus;
  eventstatus<<"Trigger events ="<<  m_data->NumEvents;
  m_data->vars.Set("Status", eventstatus.str());
  
  TTree *tree=m_data->GetTTree("PMTData");
  //std::cout<<"Debug 2"<<std::endl;

  if (tree->GetEntriesFast()>m_TTreeEventCap){
    //std::cout<<"Debug 3"<<std::endl;
    //    root_thread_args *args=new root_thread_args(OutFile,tree);
    //std::cout<<"Debug 4 "<<tree<<std::endl;
    
    
  
    std::stringstream TTreepointer;
    TTreepointer<<"TTree "<<tree;
    
    //std::cout<<"sending "<<TTreepointer.str()<<std::endl;
    zmq::message_t message(TTreepointer.str().length()+1);
    snprintf ((char *) message.data(), TTreepointer.str().length()+1 , "%s" ,TTreepointer.str().c_str()) ;
    Isend->send(message);

    //m_data->trigdata->tree->GetEntry(0);
    //std::cout<<"testing event size = "<<m_data->trigdata->EventSize<<std::endl;

    std::stringstream TTreepointer2;
    TTreepointer2<<"TTree "<<m_data->trigdata->tree;

    //std::cout<<"sending trig tree pointer "<<TTreepointer2.str()<<std::endl;
    zmq::message_t message2(TTreepointer2.str().length()+1);
    snprintf ((char *) message2.data(), TTreepointer2.str().length()+1 , "%s" ,TTreepointer2.str().c_str()) ;
    Isend->send(message2);

    //delete m_data->trigdata;
    //m_data->trigdata=0; 
    m_data->trigdata->Init();    

    //std::cout<<"sent "<<std::endl;
    //    pthread_create (&thread[0], NULL, RootDataRecorder::WriteOut, args
    //std::cout<<"Debug 5"<<std::endl;
    m_treenum++;
    std::stringstream tmp;
    // tmp<<"PMTData"<<m_treenum;
    tmp<<"PMTData";
    //std::cout<<"Debug 6 m_tmp = "<<tmp.str()<<std::endl;
    tree=new TTree(tmp.str().c_str(),tmp.str().c_str());
    //std::cout<<"Debug 7"<<std::endl;
    m_data->AddTTree("PMTData",tree);
    //std::cout<<"Debug 8"<<std::endl;
    TBranch *br;
    br=tree->Branch("LastSync",&(m_card.LastSync),"LastSync/l");
    br->SetCompressionLevel(1);  
    br=tree->Branch("SequenceID",&(m_card.SequenceID),"SequenceID/I");
    br->SetCompressionLevel(1);    
    br=tree->Branch("StartTimeSec",&(m_card.StartTimeSec),"StartTimeSec/I");
    br->SetCompressionLevel(1);    
    br=tree->Branch("StartTimeNSec",&(m_card.StartTimeNSec),"StartTimeNSec/I");
    br->SetCompressionLevel(1);    
    br=tree->Branch("StartCount",&(m_card.StartCount),"StartCount/l");
    br->SetCompressionLevel(1);   
    br=tree->Branch("TriggerNumber",&(m_card.triggerNumber),"TriggerNumber/I");
    br->SetCompressionLevel(1);   
    br=tree->Branch("TriggerCounts",m_card.triggerCounts,"TriggerCounts[TriggerNumber]/l");
    br->SetCompressionLevel(1);    
    //  tree->Branch("TriggerCounts",&(m_card.triggerCounts));
    //tree->Branch("Rates",&(m_card.Rates));
    br=tree->Branch("CardID",&(m_card.CardID),"CardID/I");
    br->SetCompressionLevel(1);   
    br=tree->Branch("Channels",&(m_card.channels),"Channels/I");
    br->SetCompressionLevel(1);    
    br=tree->Branch("Rates",m_card.Rates,"Rates[Channels]/i");
    br->SetCompressionLevel(1);
    //tree->Branch("PMTID",m_card.PMTID,"PMTID[Channels]/I");
    br=tree->Branch("BufferSize",&(m_card.buffersize),"BufferSize/I");
    br->SetCompressionLevel(1);    
    br=tree->Branch("Eventsize",&(m_card.eventsize),"Eventsize/I");
    br->SetCompressionLevel(1);    
    br=tree->Branch("FullBufferSize",&(m_card.fullbuffsize),"FullBufferSize/I");
    br->SetCompressionLevel(1);    
    //tree->Branch("Data",&(m_card.Data));
    br=tree->Branch("Data",m_card.Data,"Data[FullBufferSize]/s");
    br->SetCompressionLevel(1);    
    //std::cout<<"i d5"<<std::endl;
    m_data->AddTTree("PMTData",tree);
    //std::cout<<"Debug 9"<<std::endl;
  }


  //std::cout<<"Debug 10 "<<m_data->carddata.size()<<std::endl;
  for(int i=0;i<m_data->carddata.size();i++){
    //std::cout<<"Debug 11"<<std::endl;
    m_card.LastSync=m_data->carddata.at(i)->LastSync;
    m_card.SequenceID=m_data->carddata.at(i)->SequenceID;
    m_card.StartTimeSec=m_data->carddata.at(i)->StartTimeSec;
    m_card.StartTimeNSec=m_data->carddata.at(i)->StartTimeNSec;
    m_card.StartCount=m_data->carddata.at(i)->StartCount;
    m_card.triggerNumber=m_data->carddata.at(i)->triggerNumber;

    memcpy(m_card.triggerCounts, m_data->carddata.at(i)->triggerCounts, m_card.triggerNumber*sizeof(uint64_t));
    memcpy(m_card.Rates, m_data->carddata.at(i)->Rates, m_card.triggerNumber*sizeof(uint32_t));
    //m_card.triggerCounts=m_data->carddata.at(i)->triggerCounts;
    //m_card.Rates=m_data->carddata.at(i)->Rates;

    m_card.CardID=m_data->carddata.at(i)->CardID;
    m_card.channels=m_data->carddata.at(i)->channels;
    m_card.buffersize=m_data->carddata.at(i)->buffersize;
    m_card.eventsize=m_data->carddata.at(i)->eventsize;
    m_card.fullbuffsize=m_data->carddata.at(i)->fullbuffsize;
    
    memcpy(m_card.Data, m_data->carddata.at(i)->Data, m_card.fullbuffsize*(sizeof(uint16_t)) );
    //m_card.Data=m_data->carddata.at(i)->Data;


    /*
//std::cout<<"Debug 12"<<std::endl;
    for(int j=0;j<m_card.channels;j++){
      //std::cout<<"Debug 13"<<std::endl;

     (m_card.PMTID)[j]=(m_data->carddata.at(i)->PMTID)[j]; // these pointers have changed hence the crash
     //std::cout<<"Debug 14"<<std::endl;
      
    }	
    //std::cout<<"Debug 15"<<std::endl;
    
    for(int j=0;j<m_card.fullbuffsize;j++){
      //std::cout<<"Debug 16"<<std::endl;
      
      (m_card.Data[j])=(m_data->carddata.at(i)->Data)[j];
      //  std::cout<<m_data->carddata.at(i)->Data[j]<<" Debug 17 "<<m_card.Data[j]<<std::endl;

    }
    
    */
    //std::cout<<"Debug 18 i= "<<m_data->NumEvents<<std::endl;
     tree->Fill();
     //std::cout<<"Debug 19"<<std::endl;
  }    
  
  //std::cout<<"Debug 20"<<std::endl;
  
  ///////////////////////trigger data////////////////
  
  //std::cout<<"Filling tree event size = "<<m_data->trigdata->EventSize<<std::endl;
  m_data->trigdata->Fill();

  }
  }
  return true;
}


bool RootDataRecorder::Finalise(){

  m_data->vars.Set("Status", "");

  std::string send="Quit 0x00";
  zmq::message_t message(send.length()+1);
  //std::cout<<"sending "<<send<<std::endl;

  snprintf ((char *) message.data(), send.length()+1 , "%s" ,send.c_str()) ;
  Isend->send(message);
  //std::cout<<"sent quit "<<send<<std::endl;
  //TThread::Ps();
  h1->Join();
  //std::cout<<"joined "<<send<<std::endl;
  //  (void) pthread_join(thread[0], NULL);
  m_filepart++;  
  std::stringstream tmp;
  tmp<<OutFile<<"p"<<m_filepart<<".root";
  TFile file(tmp.str().c_str(),"UPDATE","",1);
  
  //TTree *treeorig=m_data->GetTTree("PMTData");
  //TTree *tree=treeorig->CloneTree(); 
  TTree *tree=m_data->GetTTree("PMTData");  
  tree->Write();
  // treeorig=m_data->GetTTree("RunInformation");
  //tree=treeorig->CloneTree();
  tree=m_data->GetTTree("RunInformation");
  tree->Write();

  //std::cout<<"starting write of trigtree"<<std::endl;
  m_data->trigdata->Write();

  //std::cout<<"written main trees "<<send<<std::endl;
  //////////////////////////////////MRD Tree/////////////////////////////////
  if (MRDEnable){
    zmq::socket_t MRD(*(m_data->context), ZMQ_REQ);
    int a=200000;
    MRD.setsockopt(ZMQ_RCVTIMEO, a);
    MRD.setsockopt(ZMQ_SNDTIMEO, a);
    //MRD.bind("inproc://MRDTree");
    MRD.bind("ipc:///tmp/0");
    
    
    tmp.str("");
    tmp << "Gimme";
    zmq::message_t msg(tmp.str().length()+1);
    TTree *mrdtree;
    //TTree *mrdtreeorig;
    //std::cout<<"sending mrd "<<tmp.str()<<std::endl;
    snprintf ((char *) msg.data(), tmp.str().length()+1, "%s", tmp.str().c_str());
    if (MRD.send(msg)){
      zmq::message_t txt;
     //std::cout<<"sent mrd "<<send<<std::endl;
      if (MRD.recv(&txt)){
	//std::cout<<"received mrd "<<send<<std::endl;
	std::istringstream iss(static_cast<char*>(txt.data()));
	std::string arg1;
	unsigned long arg2;

	iss >> arg1 >> std::hex >> arg2;
	
	
	if (arg1 == "TTree"){
	 //std::cout<<"in tree assignement "<<std::endl;
	  //	  mrdtreeorig=reinterpret_cast<TTree *>(arg2);
	  //	std::cout<<"cloning "<<std::endl;
	  // mrdtree=mrdtreeorig->CloneTree();
	  mrdtree=reinterpret_cast<TTree *>(arg2);
	  //std::cout<<"Tree is "<<mrdtree<<std::endl;
	}
      }
      else std::cout << "Root MRD Time out" << std::endl;
    }
    
    mrdtree->Write();
    //std::cout<<"before delete "<<send<<std::endl;
    // delete mrdtreeorig;
    //std::cout<<"written mrd "<<send<<std::endl;
  }
  ///////////////////////////////////////////////////////////////////////////////

  file.Write();
  file.Close();
  // std::stringstream compcommand;
  //compcommand<<"sleep 5 && tar -cf "<<tmp.str()<<".tar -C /data/output/ "<<"DataR"<<m_data->RunNumber<<"S"<<m_data->SubRunNumber<<"p"<<m_filepart<<".root &";//" && rm "<<tmp.str()<<" &"; 
  ////std::cout<<compcommand.str()<<std::endl;
   
  //  system("sleep 10  &");
   // sleep(5);
  //std::cout<<"starting cleanup "<<send<<std::endl;

  m_data->DeleteTTree("PMTData");
  m_data->DeleteTTree("RunInformation");
  tree=0;

  delete m_data->trigdata;
  m_data->trigdata=0;

  //test remove //delete mrdtree;
  // test removemrdtree=0;

  //  Isend->close();

  // too delete properly 

  delete h1;
  h1=0;

  delete Isend;
  Isend=0;
  
  delete args;
  args=0;

  //  delete m_card.PMTID;
  // m_card.PMTID=0;
  
  //delete m_card.Data;
  //m_card.Data=0;
  //std::cout<<"finnished "<<send<<std::endl;
  return true;
}

/*
void* RootDataRecorder::WriteOut(void* arg){
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


void* RootDataRecorder::RootWriter(void* arg){
  
  rootw_thread_args* args= static_cast<rootw_thread_args*>(arg);
 
  zmq::socket_t Ireceive (*(args->context), ZMQ_PULL);
  //std::cout<<"attempting connect"<<std::endl; 
 Ireceive.connect("inproc://RootWriter");
 //std::cout<<"connected"<<std::endl;

  bool running=true;
  *(args->filepart)=0;
  int treenum=0;
   
  /////////////// mrd tree socket//////////////////
  zmq::socket_t MRD(*(args->context), ZMQ_REQ);

  if (args->MRDEnable){
    //std::cout<< "attempting mrd bind"<<std::endl;
    //zmq::socket_t MRD(*(args->context), ZMQ_REQ);
    //MRD.bind("inproc://MRDTree");  
       MRD.bind("ipc:///tmp/0");
    //Test->bind("ipc:///tmp/0");
       //std::cout<<"bound"<<std::endl;
     }

  ////////////////////////////////////////////////////



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
    //    TTree *treeorig;    
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
      //treeorig=reinterpret_cast<TTree *>(arg2);
      tree=reinterpret_cast<TTree *>(arg2); 
      std::stringstream tmp;
      tmp<<args->OutFile<<"p"<<*(args->filepart)<<".root";
      TFile file(tmp.str().c_str(),"UPDATE","",1);
      //std::cout<<"tree = "<<tree<<std::endl;
      //tree=treeorig->CloneTree();
      tree->Write();
      TTree *ri=args->runinformation->CloneTree();
      //TTree *ri=args->runinformation;
      ri->Write();
      //std::cout<<"saved waterpmt tree"<<std::endl;
      ////////////////////////////////// trigrddata tree////////////////////

      //std::cout<<"about to receive trig tree"<<std::endl;
      zmq::message_t comm2;
      //std::cout<<"T  Debug 2"<<std::endl;
      Ireceive.recv(&comm2);
      //std::cout<<"got trigtree message"<<std::endl;

      std::istringstream iss2(static_cast<char*>(comm2.data()));
      std::string arg12="";
      void* arg22;
      //lon arg2="";
     
      //    TTree *treeorig;
      iss2>>arg12>>std::hex>>arg22;
      TTree *test;
      test=static_cast<TTree *>(arg22);
      int es;
      //test->SetBranchAddress("EventSize",&es);
      //test->GetEntry(0);
      //std::cout<<"EventSize = "<<es;
      //std::cout<<"about to thread write trig tree "<<test<<std::endl;   
      test->Write();
      //std::cout<<"tread wrote"<<std::endl;
      //////////////////////////////////////
    
      ///////////////////////////////MRD TTree Receive/////////////////////////////
      if(args->MRDEnable){
	//std::cout<<"debug mrd in"<<std::endl;	
	tmp.str("");
	tmp << "Gimme";
	zmq::message_t msg(tmp.str().length()+1);
	TTree *mrdtree;
	//TTree *mrdtreeorig;
	snprintf ((char *) msg.data(), tmp.str().length()+1, "%s", tmp.str().c_str());
	//std::cout<<"sending mrd gimmie"<<std::endl;
	if (MRD.send(msg)){
	  //std::cout<<"sent"<<std::endl;
	  zmq::message_t txt;
	  //std::cout<<"receiving mrd data"<<std::endl;
	  if (MRD.recv(&txt)){
	    //std::cout<<"mrd received"<<std::endl;
	    std::istringstream iss(static_cast<char*>(txt.data()));
	    std::string arg1;
	    unsigned long arg2;
	    
	    iss >> arg1 >> std::hex >> arg2;
	    
	    
	    if (arg1 == "TTree"){
	      //std::cout<<"received TTree"<<std::endl;
	      //	      mrdtreeorig=reinterpret_cast<TTree *>(arg2);
	      mrdtree=reinterpret_cast<TTree *>(arg2);
	    //mrdtree=mrdtreeorig->CloneTree();
	  }
	}
	  else std::cout << "Time out" << std::endl;
      }
	//std::cout<<"mrdwriting"<<std::endl;
	mrdtree->Write();
	//delete mrdtreeorig;
      }
      ////////////////////////////////////////////////////////
      
      //std::cout<<"T  Debug 5"<<std::endl;
      file.Write();
      //std::cout<<"T Debug 6"<<std::endl;
      file.Close();
      //std::cout<<"T  Debug 7"<<std::endl;
      //delete test;
      // test=0;
      //std::cout<<"T  Debug 8"<<std::endl;
      //delete treeorig;

      // std::stringstream compcommand;
      //compcommand<<"tar -cf "<<tmp.str()<<".tar -C /data/output/ "<<args->filename<<"p"<<args->filepart<<".root &";//" && rm "<<tmp.str()<<" &";                                           
      //std::cout<<compcommand.str()<<std::endl;                                    
      // system(compcommand.str().c_str());
      
      //std::cout<<"T  Debug 7"<<std::endl;
      //sleep(5);
      //std::cout<<"T  Debug 8"<<std::endl;
      //test remove
      //delete tree;
      // ri->~TTree();
      //std::cout<<"T  Debug 9"<<std::endl;
      //test remove //tree=0;
      //std::cout<<"T  Debug 10"<<std::endl;
      //test remove //delete mrdtree;
      //test //remove mrdtree=0;
      
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
