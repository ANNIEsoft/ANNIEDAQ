#include <PEDInfo.h>

PEDInfo::PEDInfo():Plot(){};


bool PEDInfo::Initialise(Store* variables, std::map<int, std::map<int, PMT> >* PMTInfo){

  init=true;

  variables->Get("OutputPath",outpath);
  
  PMTinfo=PMTInfo;
  c1= new TCanvas("c1","c1",600,400);
  
  return true;
 
}


bool  PEDInfo::Execute(std::vector<CardData*> *data){

  //std::cout<<"in orig execute"<<std::endl;
  
  ////////////////////////////////////////////////////////////
  ////////// Setting up plots/////////
  ////////////////////////////////////////////////////////////////
  std::vector<TH1I> freqplots;
  //////////////////////////////////////////////////////////////////	\
  //////
  
  for(int i=0;i<data->size();i++){
    
    if(init){      
      for(int j=0;j<data->at(i)->channels;j++){
       
	PEDmean[data->at(i)->CardID][j].resize(100,0.0);
	PEDsigma[data->at(i)->CardID][j].resize(100,0.0);
	std::stringstream tmp;
        tmp<<"Channel "<<(i*4)+j<<" frequency";
	PEDhist[data->at(i)->CardID][j]=new TH1F(tmp.str().c_str(),tmp.str().c_str(),100,0,99); 
	/*	std::stringstream tmp;
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
	*/
      }
      
    }
    
    
  
  
    for(int j=0;j<data->at(i)->channels;j++){
      
      //////////Make freq plot///////////////
      tmp.str("");
      tmp<<"Channel "<<(i*4)+j<<" frequency";
      TH1I freq(tmp.str().c_str(),tmp.str().c_str(),200,200,399);
      
      for(int k=0;k<data->at(i)->buffersize;k++){
	
	freq.Fill(data->at(i)->Data[(j*data->at(i)->buffersize)+k]);
       
      }

      freqplots.push_back(freq);
      //////// find pedistall fill ped temporals//////////

      PEDmean[data->at(i)->CardID][j].pop_front();
      PEDsigma[data->at(i)->CardID][j].pop_front();
      /*
      if (data->at(i)->CardID==21 && j==2) {
	PEDmean[21][2].push_back(0);
	PEDsigma[21][2].push_back(0);
	(*PEDinfo)[21][2].PEDmean=0;
	(*PEDinfo)[21][2].PEDsigma=0;
      }
      else{
	
      */
      freq.Fit("gaus");
	TF1 *gaus = freq.GetFunction("gaus");
	PEDmean[data->at(i)->CardID][j].push_front(gaus->GetParameter(1));
	PEDsigma[data->at(i)->CardID][j].push_front(gaus->GetParameter(2));
        (*PEDinfo)[data->at(i)->CardID][j].PEDmean=(gaus->GetParameter(1));
        (*PEDinfo)[data->at(i)->CardID][j].PEDsigma=(gaus->GetParameter(2));
	//}

      //std::cout<<"d11"<<std::endl;
      


	///////////////////filling pedistal history ///////////////////

	for(int bin=0;bin<99;bin++){
	  
	  PEDhist[data->at(i)->CardID][j]->SetBinContent(bin,PEDmean[data->at(i)->CardID][j].at(bin));

	}
     
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
    
    
  } /// size i


  ///////////////////////////plot ped hist //////////////////////////////

  for (std::map<int,std::map<int, TH1F> >::iterator it=PEDhist.begin(); it!=PEDhist.end(); ++it){
    TLegend leg(0.8,0.4,1.0,0.7)

    for (std::map<int, TH1F>::iterator it2=it->second.begin(); it2!=it->second.end(); ++it2){

      std::stringstream legend;
      legend<<"Channel "<<it2->first;
      leg.AddEntry(&(it2->second),legend.str().c_str(),"l");
      it2->second.SetLineColor((it2->first+1);

      if(it2->second==it->second.begin())it2->second.Draw();
	else it2->second.Draw("same");
    }
    leg.Draw();
    std::stringstream tmp;
    tmp.str("");
    tmp<<outpath<<it->first<<"pedhist.jpg";
    c1->SaveAs(tmp.str().c_str());

  }
  


  
  init=false;  
  
  
}


bool  PEDInfo::Finalise(){
  
  //std::cout<<"in finalise orig"<<std::endl;
  
  init=false;
  
  // tfreqplots.clear();
  delete c1;
  c1=0;

  PMTInfo.clear();
  PEDmeam.clear();
  PEDsigma.clear();

  for (std::map<int,std::map<int, TH1F> >::iterator it=PEDhist.begin(); it!=PEDhist.end(); ++it){
    
    for (std::map<int, TH1F>::iterator it2=it->second.begin(); it2!=it->second.end(); ++it2){

      delete it2->second;
      it2->second=0;

    }
  }
  
  PEDhist.clear();
  
  // PedTime.clear();
  //  PedRMSTime.clear();
  // rates.clear();
  //averagesize.clear();
  //tfreqplots.clear();
  //pedpars.clear();

}
