#include <iostream>
#include <Store.h>
#include <BoostStore.h>
#include <CardData.h>
#include <TriggerData.h>

int main(int argc, char **argv){

  BoostStore in(false,0);

  in.Initialise(argv[1]);
  in.Print(false);
  std::cout<<"File loaded"<<std::endl;

  /////////////////// getting PMT Data ////////////////////
  std::cout<<"PMTData test"<<std::endl; 
  BoostStore PMTData(false,2);
  std::cout<<"g1"<<std::endl; 
  in.Get("PMTData",PMTData);
  std::cout<<"PMTData loaded"<<std::endl;
  
  PMTData.Print(false);
  std::cout<<"PMTData Finished"<<std::endl;

  long entries=0;
  PMTData.Header->Get("TotalEntries",entries);
  std::cout<<"entries = "<<entries<<std::endl;
  std::cout<<"p4"<<std::endl;

  for( int i=0;i<5;i++){
    std::cout<<"entry "<<i<<" of "<<entries<<std::endl;
    PMTData.GetEntry(i);
    std::vector<CardData> Cdata;
    PMTData.Get("CardData",Cdata);
    std::cout<<"Cdata size="<<Cdata.size()<<std::endl;
    std::cout<<"Cdata entry 0 CardID="<<Cdata.at(0).CardID<<std::endl;
    std::cout<<"Cdata entry 0 wave form of size="<<Cdata.at(0).Data.size()<<std::endl;
    for (int j=0;j<Cdata.at(0).Data.size();j++){
      std::cout<<Cdata.at(0).Data.at(j)<<" , ";
    }
    std::cout<<std::endl;

  }
  ///////////////////////////////////////////

  ////////////////////////getting trigger data ////////////////
  std::cout<<"TrigData test"<<std::endl;
  BoostStore TrigData(false,2);
  in.Get("TrigData",TrigData);
  std::cout<<"Trig Data Loade"<<std::endl;

  TrigData.Print(false);
  long trigentries=0;
  TrigData.Header->Get("TotalEntries",trigentries);
  std::cout<<"entries = "<<entries<<std::endl;

  for( int i=0;i<5;i++){
    std::cout<<"entry "<<i<<" of "<<trigentries<<std::endl;
    TrigData.GetEntry(i);
    TriggerData Tdata;
    TrigData.Get("TrigData",Tdata);  
    int EventSize=Tdata.EventSize;                                                  
    std::cout<<"EventSize="<<EventSize<<std::endl;
    std::cout<<"SequenceID="<<Tdata.SequenceID<<std::endl;
    std::cout<<"EventIDs size="<<Tdata.EventIDs.size()<<std::endl;  
    for(int j=0;j<Tdata.EventIDs.size();j++){
      
     std::cout<< Tdata.EventIDs.at(j)<<" , ";  
    }
    std::cout<<std::endl;						
		
  }

  return 0;

}
