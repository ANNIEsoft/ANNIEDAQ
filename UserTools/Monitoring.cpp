#include "Monitoring.h"

Monitoring::Monitoring():Tool(){}


bool Monitoring::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  std::string MonitorPort;
  m_variables.Get("MonitorLevel",MonitoringLevel);

  Isend= new zmq::socket_t(*(m_data->context),ZMQ_PAIR);
  Isend->bind("inproc://MonitorThread");

  args=new monitor_thread_args(m_data->context,*(m_variables["OutputPath"]));
  // pthread_create (&thread, NULL, Monitoring::MonitorThread, args);
  h1 = new TThread("h1", Monitoring::MonitorThread, args);
  h1->Run();
  //  int i=system("rm /data/monitoringplots/* &");
  

  return true;

}


bool Monitoring::Execute(){

  if( m_data->triggered){

   //std::cout<<"m_data->NumEvents "<<m_data->NumEvents<<std::endl;
   //std::cout<<"MonitoringLevel "<<MonitoringLevel<<std::endl;
    //std::cout<<"(m_data->NumEvents % MonitoringLevel) "<<(m_data->NumEvents%MonitoringLevel)<<std::endl;
    
    if ((m_data->NumEvents%MonitoringLevel)==0){
      // std::cout<<"in"<<std::endl;   
      std::stringstream data;
      data<<"Data "<<m_data->carddata.size()<<" ";

      for(int i=0;i<m_data->carddata.size();i++){
	data<<m_data->carddata.at(i)<<" ";
	m_data->carddata.at(i)=0;
      }
      //  std::cout<<"message sending = "<<data.str()<<std::endl;
      zmq::message_t message(data.str().length()+1);
      
      snprintf ((char *) message.data(), data.str().length()+1 , "%s" ,data.str().c_str()) ;
      Isend->send(message);
      //std::cout<<"out"<<std::endl; 
    }
    
  }
  
  return true;
}


bool Monitoring::Finalise(){


  zmq::message_t message(256);
  snprintf ((char *) message.data(), 256 , "%s" ,"Quit") ;
  Isend->send(message);
  
  //  (void) pthread_join(thread, NULL);
  h1->Join();  

  delete h1;
  h1=0;

  delete Isend;
  Isend=0;

  delete args;
  args=0;

  return true;
}


void* Monitoring::MonitorThread(void* arg){
  
  monitor_thread_args* args= static_cast<monitor_thread_args*>(arg);
  
  std::string outpath=args->outputpath;
  zmq::socket_t Ireceive (*(args->context), ZMQ_PAIR);
  Ireceive.connect("inproc://MonitorThread");
  
  //  std::vector<CardData*> carddata;
  std::vector<TH1I> freqplots;
  std::map<int,std::vector<std::vector<float > > > pedpars;
  TCanvas c1("c1","c1",600,400);
  
  
  bool running=true;
  bool init=true;    

  std::vector<PMT> PMTInfo;
  /////////////////// Connect to sql ///////////////////////

  pqxx::connection *C;
  std::stringstream tmp;
  tmp<<"dbname=annie"<<" hostaddr=127.0.0.1"<<" port=5432" ;
  C=new pqxx::connection(tmp.str().c_str());
  if (C->is_open()) {
    // std::cout << "Opened database successfully: " << C->dbname() << std::endl;
  }
  else {
    std::cout << "Can't open database" << std::endl;
    return false;
  }

  tmp.str("");

  pqxx::nontransaction N(*C);

  tmp<<"select gx,gy,gz,vmecard,vmechannel from pmtconnections order by channel; ";


  /* Execute SQL query */
  pqxx::result R( N.exec( tmp.str().c_str() ));

  //pqxx::result::const_iterator c = R.begin();

  for ( pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) {

    PMT tmp;
    tmp.gx= c[0].as<int>();
    tmp.gy= c[1].as<int>();
    tmp.gz= c[2].as<int>();
    tmp.card= c[3].as<int>();
    tmp.channel= c[4].as<int>()-1;
    PMTInfo.push_back(tmp);
  }


  
  while (running){
    
    
    zmq::message_t comm;
    Ireceive.recv(&comm);
	
    std::istringstream iss(static_cast<char*>(comm.data()));
    std::string arg1="";
    iss>>arg1;
    
    if(arg1=="Data"){
      std::vector<TGraph2D*> mg;
      TH2I EventDisplay ("Event Display", "Event Display", 10, -1, 8, 10, -1, 8);
      std::vector<TH1F> temporalplots;
      CardData* carddata;
      int size=0;
      iss>>size;  
      
      //freqplots.clear();
      
      for(int i=0;i<size;i++){

	long long unsigned int pointer;
	iss>>std::hex>>pointer;
	
	carddata=(reinterpret_cast<CardData *>(pointer));
	
	if(init){
	  for(int j=0;j<carddata->channels;j++){
	    std::stringstream tmp;
	    tmp<<"Channel "<<(i*4)+j<<" frequency";
	    TH1I tmpfreq(tmp.str().c_str(),tmp.str().c_str(),200,200,399);
	    freqplots.push_back(tmpfreq);
	    std::vector<float> tmppedpars;
	    tmppedpars.push_back(0);
	    tmppedpars.push_back(0);
	    pedpars[carddata->CardID].push_back(tmppedpars);
	  }
	  if(i==size-1)init=false;
	}

	//	std::cout<<"d1"<<std::endl;

	for(int j=0;j<carddata->channels;j++){
	  std::stringstream tmp;
	  tmp<<"Channel "<<(i*4)+j<<" temporal";
	  
	  TH1F temporal(tmp.str().c_str(),tmp.str().c_str(),carddata->buffersize,0,carddata->buffersize-1);
	  long sum=0;
	
	  //  std::cout<<"d2"<<std::endl;
	
	for(int k=0;k<carddata->buffersize;k++){
	  //	  std::cout<<"i="<<i<<" j="<<j<<std::endl;
	  //std::cout<<"d2.5 "<<(i*4)+j<<" feqplot.size = "<<freqplots.size()<<std::endl;
	  if(carddata->Data[(j*carddata->buffersize)+k]>pedpars[carddata->CardID].at(j).at(0)+(pedpars[carddata->CardID].at(j).at(1)*5))sum+=carddata->Data[(j*carddata->buffersize)+k];  
	  freqplots.at((i*4)+j).Fill(carddata->Data[(j*carddata->buffersize)+k]);

	  //temporal.SetBinContent(k,carddata->Data[(j*carddata->buffersize)+k]);	      
	}

	freqplots.at((i*4)+j).Fit("gaus");
	TF1 *gaus = freqplots.at((i*4)+j).GetFunction("gaus");
        pedpars[carddata->CardID].at(j).at(0)=(gaus->GetParameter(1));
        pedpars[carddata->CardID].at(j).at(1)=(gaus->GetParameter(2));
	gaus->SetLineColor(j+1);
       

	
	for(int k=0;k<carddata->buffersize/4;k++){
          //std::cout<<"j*4 = "<<j*4<<std::endl;
          //std::cout<<"(i*BufferSize)+(j*4) = "<<(i*BufferSize)+(j*4)<<std::endl;
          //std::cout<<"i*BufferSize)+(j*4)+(BufferSize/2) = "<<(i*BufferSize)+(j*4)+(BufferSize/2)<<std::endl;
          //std::cout<<"(i*BufferSize)+(j*4)+(BufferSize/2)+1 = "<<(i*BufferSize)+(j*4)+(BufferSize/2)+1<<std::endl;
	  int offset=pedpars[carddata->CardID].at(j).at(0);
          double conversion=2.415/pow(2.0, 12.0);
	  temporal.SetBinContent(k*4,(carddata->Data[(j*carddata->buffersize)+(k*2)]-offset)*conversion);
	  temporal.SetBinContent((k*4)+1,(carddata->Data[(j*carddata->buffersize)+(k*2)+1]-offset)*conversion);
	  temporal.SetBinContent((k*4)+2,(carddata->Data[(j*carddata->buffersize)+(k*2)+(carddata->buffersize/2)]-offset)*conversion);
	  temporal.SetBinContent((k*4)+3,(carddata->Data[(j*carddata->buffersize)+(k*2)+(carddata->buffersize/2)+1]-offset)*conversion);

        }


	//std::cout<<"d3"<<std::endl;
	
	temporalplots.push_back(temporal);

	int x=-10;
	int z=-10;
	int y=-10;
	for(int pmt=0;pmt<PMTInfo.size();pmt++){

	  if(PMTInfo.at(pmt).card==carddata->CardID && PMTInfo.at(pmt).channel==j){
	    x=PMTInfo.at(pmt).gx;
	    z=PMTInfo.at(pmt).gz;
	    y=PMTInfo.at(pmt).gy;
	  }
	}

	/*
	int x=(((i*4)+j)%8);
	int y=(floor(((i*4)+j)/8.0));
	if(x==0 && y==0){x=-10;y=-10;}
	if(x==7 && y==0){x=-10;y=-10;}
	if(x==0 && y==7){x=-10;y=-10;}
	if(x==7 && y==7){x=-10;y=-10;}
	if (y>7){x=-10;y=-10;}
	std::cout<<"i="<<i<<" j="<<j<<" (i*4)+j)="<<((i*4)+j)<<" x="<<x<<" y="<<y<<" sum="<<sum<<std::endl;
	EventDisplay.SetBinContent(x+1,y+1,sum);
	//EventDisplay.SetBinContent(((i*4)+j),sum);
	*/
	if(x!=-10 && z!=-10){
	  //std::cout<<"gx = "<<x<<" , gz="<<z<<std::endl;
	  EventDisplay.SetBinContent(x+2,z+2,sum);

	  TGraph2D *dt=new TGraph2D(1);
	  dt->SetPoint(0,x,z,z);
	  dt->SetMarkerStyle(20);
	  //dt->GetXaxis()->SetRangeUser(-1,8);
	  // dt->GetYaxis()->SetRangeUser(-1,8);
	  //dt->GetZaxis()->SetRangeUser(-1,8);
	  mg.push_back(dt);
	}	
	
	}
	
	
	int maxplot=0;
	long maxvalue=0;
	
	//std::cout<<"i="<<i<<" (i*4)="<<(i*4)<<" (i*4)+4="<<(i*4)+4<<" size="<<freqplots.size()<<std::endl;
	for(int j=(i*4);j<(i*4)+4;j++){
	  if (freqplots.at(j).GetMaximum()>maxvalue){
	    maxvalue=freqplots.at(j).GetMaximum();
	    maxplot=j;
	  }
	}
	
	time_t t = time(0);   // get time now
	struct tm * now = localtime( & t );
	std::stringstream title;
	title<<"Card "<<carddata->CardID<<" frequency: "<<(now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' <<  now->tm_mday<<','<<now->tm_hour<<':'<<now->tm_min<<':'<<now->tm_sec;
	freqplots.at(maxplot).SetTitle(title.str().c_str());	
	freqplots.at(maxplot).SetLineColor((maxplot%4)+1);
	freqplots.at(maxplot).Draw();
	TLegend leg(0.8,0.4,1.0,0.7);
	//leg.SetHeader("The Legend Title");
	for(int j=(i*4);j<(i*4)+4;j++){
	  std::stringstream legend;
	  legend<<"Channel "<<j-(i*4);
	  leg.AddEntry(&freqplots.at(j),legend.str().c_str(),"l");	  
	  freqplots.at(j).SetLineColor((j%4)+1);
	  if(j==maxplot){;}//freqplots.at(i).Draw();                                  
	  else freqplots.at(j).Draw("same");
	  
	}
	leg.Draw();

	std::stringstream tmp;
	tmp<<outpath<<carddata->CardID<<"freq.jpg";
	c1.SaveAs(tmp.str().c_str());


	////temporal
	maxplot=0;
	maxvalue=0;

	//std::cout<<"i="<<i<<" (i*4)="<<(i*4)<<" (i*4)+4="<<(i*4)+4<<" size="<<freqplots.size()<<std::endl;
        for(int j=(i*4);j<(i*4)+4;j++){
          if (temporalplots.at(j).GetMaximum()>maxvalue){
            maxvalue=temporalplots.at(j).GetMaximum();
            maxplot=j;
          }
        }

	t = time(0);   // get time now
        now = localtime( & t );
	std::stringstream title2;
        title2<<"Card "<<carddata->CardID<<" Temporal: "<<(now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' <<  now->tm_mday<<','<<now->tm_hour<<':'<<now->tm_min<<':'<<now->tm_sec;
        temporalplots.at(maxplot).SetTitle(title2.str().c_str());
        temporalplots.at(maxplot).SetLineColor((maxplot%4)+1);
        temporalplots.at(maxplot).Draw();
	TLegend leg2(0.8,0.4,1.0,0.7);
	for(int j=(i*4);j<(i*4)+4;j++){
	  std::stringstream legend;
	  legend<<"Channel "<<j-(i*4);	  
	  leg2.AddEntry(&temporalplots.at(j),legend.str().c_str(),"l");
	  temporalplots.at(j).SetLineColor((j%4)+1);
	  if(j==0){;}
	  else temporalplots.at(j).Draw("same");

	}
	leg2.Draw();
	//std::cout<<"d6"<<std::endl;

	std::stringstream tmp2;
	tmp2<<outpath<<carddata->CardID<<"temporal.jpg";
	c1.SaveAs(tmp2.str().c_str());



	delete carddata;

	
      } /// size i
      
      /*
      //std::cout<<"d4"<<std::endl;

      int maxplot=0;
      long maxvalue=0;
      for(int i=0;i<freqplots.size();i++){
	if (freqplots.at(i).GetMaximum()>maxvalue){
	  maxvalue=freqplots.at(i).GetMaximum();
	  maxplot=i;
	}
      }

      freqplots.at(maxplot).SetLineColor(maxplot+1);
      freqplots.at(maxplot).Draw();
      for(int i=0;i<freqplots.size();i++){

	//	Double_t scale = 1/freqplots.at(i).GetMaximum();
	//	freqplots.at(i).Scale(scale);
	freqplots.at(i).SetLineColor(i+1);
	if(i==maxplot);//freqplots.at(i).Draw();
	else freqplots.at(i).Draw("same");

	//freqplots.at(i).Scale((1.0/scale));
      }
      std::cout<<"d5"<<std::endl;

      std::stringstream tmp;
      tmp<<outpath<<"freq.jpg";
      c1.SaveAs(tmp.str().c_str());
      
      for(int i=0;i<temporalplots.size();i++){
	temporalplots.at(i).SetLineColor(i+1);
	if(i==0)temporalplots.at(i).Draw();
	else temporalplots.at(i).Draw("same");
	
      }	  

      std::cout<<"d6"<<std::endl;

      std::stringstream tmp2;
      tmp2<<outpath<<"temporal.jpg";
      c1.SaveAs(tmp2.str().c_str());
      */
      EventDisplay.Draw("COLZ");
      std::stringstream tmp3;
      tmp3<<outpath<<"0EventDisplay.jpg";
      c1.SaveAs(tmp3.str().c_str());
      

      tmp3.str("");
      mg.at(0)->Draw();
      for(int plots=1;plots<mg.size();plots++){
	mg.at(plots)->Draw("same");
      }
      tmp3<<outpath<<"0EventDisplay3D.jpg";
      c1.SaveAs(tmp3.str().c_str());

    }
    
    
    
    else if(arg1=="Quit"){
      freqplots.clear();
      running=false;
      
    }
    
  }
  
  
}
