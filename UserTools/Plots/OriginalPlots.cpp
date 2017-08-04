#include <OriginalPlots.h>

OriginalPlots::OriginalPlots():Plot(){};


bool OriginalPlots::Initialise(Store* variables){

  init=true;

  c1=new TCanvas("c1","c1",600,400);
  variables->Get("OutputPath",outpath);
  /////////////////// Connect to sql ///////////////////////
  //std::cout<<"d2 in original initialise "<<outpath<<std::endl;

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


  ///////// Fill PMT Info////////////////
  for ( pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) {

    PMT tmp;
    tmp.gx= c[0].as<int>();
    tmp.gy= c[1].as<int>();
    tmp.gz= c[2].as<int>();
    tmp.card= c[3].as<int>();
    tmp.channel= c[4].as<int>()-1;
    PMTInfo.push_back(tmp);
  }

  // disconnect and delete sql C
  C->disconnect();
  delete C;
  C=0;
}


bool  OriginalPlots::Execute(std::vector<CardData*> *data){

  //std::cout<<"in orig execute"<<std::endl;
  
  ////////////////////////////////////////////////////////////
  ////////// Setting up plots/////////
  ////////////////////////////////////////////////////////////////
  std::vector<TGraph2D*> mg;
  TH2I EventDisplay ("Event Display", "Event Display", 10, -1, 8, 10, -1, 8);
  TH2I RMSDisplay ("RMS Display", "RMS Display", 10, -1, 8, 10, -1, 8);
  std::vector<TH1F> temporalplots;
  std::vector<TH1I> freqplots;
  //////////////////////////////////////////////////////////////////  \
  //////
  
  for(int i=0;i<data->size();i++){
    
    if(init){      
      for(int j=0;j<data->at(i)->channels;j++){
	std::stringstream tmp;
	tmp<<"Channel "<<(i*4)+j<<" frequency";
	TH1I tmpfreq(tmp.str().c_str(),tmp.str().c_str(),10,0,9);
	tfreqplots.push_back(tmpfreq);
	
	tmp.str("");
	tmp<<"Channel "<<(i*4)+j<<" Pedistal";
	TH1F tmppedtime(tmp.str().c_str(),tmp.str().c_str(),100,0,99);
	PedTime[data->at(i)->CardID].push_back(tmppedtime);
	
	tmp.str("");
	tmp<<"Channel "<<(i*4)+j<<" Pedistal RMS";
	TH1F tmppedrmstime(tmp.str().c_str(),tmp.str().c_str(),100,0,99);
	PedRMSTime[data->at(i)->CardID].push_back(tmppedrmstime);
	std::vector<float> tmppedpars;
	tmppedpars.push_back(0);
	tmppedpars.push_back(0);
	pedpars[data->at(i)->CardID].push_back(tmppedpars);
	
      }
      
    }
    
    
    
  
    
    ///////Make temporal plot //////////
    for(int j=0;j<data->at(i)->channels;j++){
      std::stringstream tmp;
      tmp<<"Channel "<<(i*4)+j<<" temporal";
      
      TH1F temporal(tmp.str().c_str(),tmp.str().c_str(),data->at(i)->buffersize,0,data->at(i)->buffersize-1);
      long sum=0;
      
      //////////Make freq plot///////////////
      tmp.str("");
      tmp<<"Channel "<<(i*4)+j<<" frequency";
      TH1I freq(tmp.str().c_str(),tmp.str().c_str(),200,200,399);
      
      
      for(int k=0;k<data->at(i)->buffersize;k++){
	//std::cout<<"d10"<<std::endl;
	
	//      std::cout<<"i="<<i<<" j="<<j<<std::endl;
	//std::cout<<"d2.5 "<<(i*4)+j<<" feqplot.size = "<<freqplots.size()<<std::endl;
	if(data->at(i)->Data[(j*data->at(i)->buffersize)+k]>pedpars[data->at(i)->CardID].at(j).at(0)+(pedpars[data->at(i)->CardID].at(j).at(1)*5))sum+=data->at(i)->Data[(j*data->at(i)->buffersize)+k];
	freq.Fill(data->at(i)->Data[(j*data->at(i)->buffersize)+k]);
	
	//temporal.SetBinContent(k,data->at(i)->Data[(j*data->at(i)->buffersize)+k]);
      }
      
      //std::cout<<"d10b entries=" <<freq.GetEntries()<<std::endl;

      freqplots.push_back(freq);
      //////// find pedistall fill ped temporals//////////
     
	TFitResultPtr gaussFitResult = freq.Fit("gaus");
	Int_t gaussFitResultInt = gaussFitResult; // cast to an int, 0 for OK
	if (gaussFitResultInt == 0) {
		TF1 *gaus = freq.GetFunction("gaus");
		pedpars[data->at(i)->CardID].at(j).at(0)=(gaus->GetParameter(1));
		pedpars[data->at(i)->CardID].at(j).at(1)=(gaus->GetParameter(2));
		gaus->SetLineColor(j+1);
	}
      

      //std::cout<<"d11"<<std::endl;
      
      for(int bin=99;bin>0;bin--){
	PedTime[data->at(i)->CardID].at(j).SetBinContent(bin,PedTime[data->at(i)->CardID].at(j).GetBinContent(bin-1));
	PedRMSTime[data->at(i)->CardID].at(j).SetBinContent(bin,PedRMSTime[data->at(i)->CardID].at(j).GetBinContent(bin-1));
      }
      PedTime[data->at(i)->CardID].at(j).SetBinContent(0, pedpars[data->at(i)->CardID].at(j).at(0));
      PedRMSTime[data->at(i)->CardID].at(j).SetBinContent(0, pedpars[data->at(i)->CardID].at(j).at(1));
      
      //////// fill temporal plot/////////
      for(int k=0;k<data->at(i)->buffersize/4;k++){
	//std::cout<<"d12"<<std::endl;
	
	//std::cout<<"j*4 = "<<j*4<<std::endl;
	//std::cout<<"(i*BufferSize)+(j*4) = "<<(i*BufferSize)+(j*4)<<std::endl;
	//std::cout<<"i*BufferSize)+(j*4)+(BufferSize/2) = "<<(i*BufferSize)+(j*4)+(BufferSize/2)<<std::endl;
	//std::cout<<"(i*BufferSize)+(j*4)+(BufferSize/2)+1 = "<<(i*BufferSize)+(j*4)+(BufferSize/2)+1<<std::endl;
	int offset=pedpars[data->at(i)->CardID].at(j).at(0);
	double conversion=2.415/pow(2.0, 12.0);
	temporal.SetBinContent(k*4,(data->at(i)->Data[(j*data->at(i)->buffersize)+(k*2)]-offset)*conversion);
	temporal.SetBinContent((k*4)+1,(data->at(i)->Data[(j*data->at(i)->buffersize)+(k*2)+1]-offset)*conversion);
	temporal.SetBinContent((k*4)+2,(data->at(i)->Data[(j*data->at(i)->buffersize)+(k*2)+(data->at(i)->buffersize/2)]-offset)*conversion);
	temporal.SetBinContent((k*4)+3,(data->at(i)->Data[(j*data->at(i)->buffersize)+(k*2)+(data->at(i)->buffersize/2)+1]-offset)*conversion);
	
      }
      
      //std::cout<<"d13"<<std::endl;
      
      //std::cout<<"d3"<<std::endl;
      
      temporalplots.push_back(temporal);
      
      
      
      ////// find x,y,z fill event display /////////
      int x=-10;
      int z=-10;
      int y=-10;
      for(int pmt=0;pmt<PMTInfo.size();pmt++){
	//std::cout<<"d4"<<std::endl;
	
	if(PMTInfo.at(pmt).card==data->at(i)->CardID && PMTInfo.at(pmt).channel==j){
	  x=PMTInfo.at(pmt).gx;
	  z=PMTInfo.at(pmt).gz;
	  y=PMTInfo.at(pmt).gy;
	  //std::cout<<"d15"<<std::endl;
	  
	}
      }
      
      if(x!=-10 && z!=-10){
	//std::cout<<"d17"<<std::endl;
	
	//std::cout<<"gx = "<<x<<" , gz="<<z<<std::endl;
	EventDisplay.SetBinContent(x+2,z+2,sum);
	RMSDisplay.SetBinContent(x+2,z+2, pedpars[data->at(i)->CardID].at(j).at(1)*100);
	
	//// Attempted 2ne event display ///
	TGraph2D *dt=new TGraph2D(1);
	dt->SetPoint(0,x,z,z);
	dt->SetMarkerStyle(20);
	//dt->GetXaxis()->SetRangeUser(-1,8);
	// dt->GetYaxis()->SetRangeUser(-1,8);
	//dt->GetZaxis()->SetRangeUser(-1,8);
	mg.push_back(dt);
      }
      
      
      //std::cout<<"d18"<<std::endl;
      
    }
    
    ///////Find max freq for scaling//////////
    
    int maxplot=0;
    long maxvalue=0;
    
    //std::cout<<"i="<<i<<" (i*4)="<<(i*4)<<" (i*4)+4="<<(i*4)+4<<"size="<<freqplots.size()<<std::endl;
    for(int j=(i*4);j<(i*4)+4;j++){
      //std::cout<<"d18b j="<<j<<" freqplots.size()="<<freqplots.size()<<std::endl;
      if (freqplots.at(j).GetMaximum()>maxvalue){
	//std::cout<<"d19"<<std::endl;
	
	maxvalue=freqplots.at(j).GetMaximum();
	maxplot=j;
      }
    }
    
    
    //std::cout<<"d20"<<std::endl;
    
    ////////Find current time and plot frewuency plot
    
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    std::stringstream title;
    title<<"Card "<<data->at(i)->CardID<<" frequency: "<<(now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' <<  now->tm_mday<<','<<now->tm_hour<<':'<<now->tm_min<<':'<<now->tm_sec;
    //std::cout<<"d21"<<std::endl;
    
    //std::cout<<"d21"<<std::endl;
    freqplots.at(maxplot).SetTitle(title.str().c_str());
    freqplots.at(maxplot).GetXaxis()->SetTitle("ADC Value");
    freqplots.at(maxplot).GetYaxis()->SetTitle("Frequency");
    freqplots.at(maxplot).SetLineColor((maxplot%4)+1);
    freqplots.at(maxplot).Draw();
    TLegend leg(0.8,0.4,1.0,0.7);
    //leg.SetHeader("The Legend Title");
    
    //std::cout<<"d22"<<std::endl;
    for(int j=(i*4);j<(i*4)+4;j++){
      //std::cout<<"d23"<<std::endl;
      std::stringstream legend;
      legend<<"Channel "<<j-(i*4);
      leg.AddEntry(&freqplots.at(j),legend.str().c_str(),"l");	
      
      freqplots.at(j).SetLineColor((j%4)+1);
      if(j==maxplot){;}//freqplots.at(i).Draw();	\
      
      else freqplots.at(j).Draw("same");
      
    }
    leg.Draw();
    //std::cout<<"d24"<<std::endl;
    std::stringstream tmp;
    tmp.str("");
    
    tmp<<outpath<<data->at(i)->CardID<<"freq.jpg";
    c1->SaveAs(tmp.str().c_str());
    
    
    //std::cout<<"d25"<<std::endl;
    
    ///////find max tmporal plot for scaling
    
    ///temporal
    maxplot=0;
    maxvalue=0;
    
    //std::cout<<"i="<<i<<" (i*4)="<<(i*4)<<" (i*4)+4="<<(i*4)+4<<"size="<<freqplots.size()<<std::endl;
    for(int j=(i*4);j<(i*4)+4;j++){
      //std::cout<<"d26a j="<<j<<std::endl;
      if (temporalplots.at(j).GetMaximum()>maxvalue){
	maxvalue=temporalplots.at(j).GetMaximum();
	maxplot=j;
      }
    }
  
    //std::cout<<"d27"<<std::endl;
    
    
    //////// Find time and plot temporal plot
    
    t = time(0);   // get time now
    now = localtime( & t );
    std::stringstream title2;
    title2<<"Card "<<data->at(i)->CardID<<" Temporal: "<<(now->tm_year+ 1900) << '-' << (now->tm_mon + 1) << '-' <<  now->tm_mday<<','<<now->tm_hour<<':'<<now->tm_min<<':'<<now->tm_sec;
    //std::cout<<"d28"<<std::endl;
    temporalplots.at(maxplot).SetTitle(title2.str().c_str());
    temporalplots.at(maxplot).GetXaxis()->SetTitle("Samples");
    temporalplots.at(maxplot).GetYaxis()->SetTitle("Volate (V)");
    temporalplots.at(maxplot).SetLineColor((maxplot%4)+1);
    temporalplots.at(maxplot).Draw();
    
    TLegend leg2(0.8,0.4,1.0,0.7);
    //std::cout<<"d29"<<std::endl;
    for(int j=(i*4);j<(i*4)+4;j++){
      //std::cout<<"d30 j="<<j<<std::endl;
      std::stringstream legend;
      legend<<"Channel "<<j-(i*4);
      leg2.AddEntry(&temporalplots.at(j),legend.str().c_str(),"l");
      temporalplots.at(j).SetLineColor((j%4)+1);
      if(j==0){;}
      else temporalplots.at(j).Draw("same");
	
    }
    //std::cout<<"d31"<<std::endl;
    leg2.Draw();
    //std::cout<<"d6"<<std::endl;
    
    std::stringstream tmp2;
    
    tmp2<<outpath<<data->at(i)->CardID<<"temporal.jpg";
    c1->SaveAs(tmp2.str().c_str());
    //std::cout<<"d32"<<std::endl;
        

    ///////plotting PED time and ped rms time //////
    maxplot=0;
    maxvalue=0;
    
    
    for(int j=0;j<4;j++){
      //std::cout<<"d26b"<<std::endl;
      if ( PedTime[data->at(i)->CardID].at(j).GetMaximum()>maxvalue){
	maxvalue=PedTime[data->at(i)->CardID].at(j).GetMaximum();
	maxplot=j;
	}
    }
      
    t = time(0);   // get time now
    now = localtime( & t );
    title2.str("");
    title2<<"Card "<<data->at(i)->CardID<<" Pedistal Variation: "<<(now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' <<  now->tm_mday<<','<<now->tm_hour<<':'<<now->tm_min<<':'<<now->tm_sec;
    //std::cout<<"d28"<<std::endl;
    PedTime[data->at(i)->CardID].at(maxplot).SetTitle(title2.str().c_str());
    PedTime[data->at(i)->CardID].at(maxplot).GetXaxis()->SetTitle("Samples");
    PedTime[data->at(i)->CardID].at(maxplot).GetYaxis()->SetTitle("ADC Value");
    PedTime[data->at(i)->CardID].at(maxplot).SetLineColor((maxplot%4)+1);
    PedTime[data->at(i)->CardID].at(maxplot).Draw();
    TLegend leg3(0.8,0.4,1.0,0.7);
    
    //std::cout<<"d29"<<std::endl;
    for(int j=0;j<4;j++){
      //std::cout<<"d30"<<std::endl;
      std::stringstream legend;
      legend<<"Channel "<<j;
      leg3.AddEntry(&PedTime[data->at(i)->CardID].at(j),legend.str().c_str(),"l");
      PedTime[data->at(i)->CardID].at(j).SetLineColor((j%4)+1);
      PedTime[data->at(i)->CardID].at(j).Draw("same");
      
    }
    //std::cout<<"d31"<<std::endl;
    leg3.Draw();
    //std::cout<<"d6"<<std::endl;
    
    tmp2.str("");
    tmp2<<outpath<<"plots2/"<<data->at(i)->CardID<<"PedTime.jpg";
    
    c1->SaveAs(tmp2.str().c_str());
    
    
    
    PedRMSTime[data->at(i)->CardID].at(0).Draw();
    for (int channel=1;channel<4;channel++){
      PedRMSTime[data->at(i)->CardID].at(channel).Draw("same");
    }
    tmp2.str("");
    tmp2<<outpath<<"plots2/"<<data->at(i)->CardID<<"PedRMSTime.jpg";
    c1->SaveAs(tmp2.str().c_str());
    
    // }
    
  } /// size i

  init=false;  
  /////////plot event display /////////
  
  EventDisplay.Draw("COLZ");
  std::stringstream tmp3;
  tmp3<<outpath<<"0EventDisplay.jpg";
  c1->SaveAs(tmp3.str().c_str());

  
  //std::cout<<"d34 ="<<mg.size()<<std::endl;
  
  /////////plot RMS display /////////
  RMSDisplay.Draw("COLZ");
  tmp3.str("");
  tmp3<<outpath<<"0RMSDisplay.jpg";
  c1->SaveAs(tmp3.str().c_str());
  
  ///plot  atempted 3d event display///////
  
  tmp3.str("");
  if(mg.size()>0){
    mg.at(0)->Draw();
    delete mg.at(0);
    mg.at(0)=0;
  }
  for(int plots=1;plots<mg.size();plots++){
    mg.at(plots)->Draw("same");
    //std::cout<<"d35"<<std::endl;
    delete mg.at(plots);
    mg.at(plots)=0;
  }
  tmp3<<outpath<<"0EventDisplay3D.jpg";
  //c1->SaveAs(tmp3.str().c_str());
  mg.clear();
  
}


bool  OriginalPlots::Finalise(){
  
  //std::cout<<"in finalise orig"<<std::endl;
  
  init=false;
  
  tfreqplots.clear();
  delete c1;
  c1=0;

  PMTInfo.clear();
  PedTime.clear();
  PedRMSTime.clear();
  rates.clear();
  averagesize.clear();
  tfreqplots.clear();
  pedpars.clear();

}
