#include <iostream>
#include <BoostStore.h>
#include <MRDOut.h>

int main(){


  BoostStore a(false,2);
  a.Initialise("test");
  a.Print(false);
  a.Header->Print();
  long entries=0;
  a.Header->Get("TotalEntries",entries);
  std::cout<<"entries = "<<entries<<std::endl;

  MRDOut data;

  for( int i=0;i<10;i++){
    a.GetEntry(i);

  a.Get("Data",data);

  std::cout<<"OutN = "<<data.OutN<<std::endl;
  std::cout<<"Trigger = "<<data.Trigger<<std::endl;
  std::cout<<"TimeStamp = "<<data.TimeStamp<<std::endl;
  std::cout<<"ValueSize = "<<data.Value.size()<<std::endl;
  std::cout<<"SlotSize = "<<data.Slot.size()<<std::endl;
  std::cout<<"ChannelSize = "<<data.Channel.size()<<std::endl;
  std::cout<<"CrateSize = "<<data.Crate.size()<<std::endl;

  for(int i=0; i<data.Value.size();i++){

    std::cout<<i<<" : value="<<data.Value.at(i)<<", slot="<<data.Slot.at(i)<<", Channel="<<data.Channel.at(i)<< ", Crate="<<data.Crate.at(i)<<std::endl;

  }

}
  unsigned int OutN, Trigger;
  std::vector<unsigned int> Value, Slot, Channel, Crate;
  std::vector<std::string> Type;
  //ULong64_t
  long TimeStamp;

  return 0;

}
