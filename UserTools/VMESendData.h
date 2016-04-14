#ifndef VMESENDATA_H
#define VMESENDATA_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Tool.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>  
#include <boost/serialization/vector.hpp>
//#include <boost/serialization/split_member.hpp>
#include <boost/serialization/split_free.hpp>

BOOST_SERIALIZATION_SPLIT_FREE(CardData);

namespace boost {
  namespace serialization {

    //   template<class Archive>
    //void serialize(Archive & ar, DataModel & d, const unsigned int version);
    //BOOST_SERIALIZATION_SPLIT_MEMBER()
    //  BOOST_SERIALIZATION_SPLIT_FREE(DataModel)
    /*template<class Archive>
      inline void serialize(
			    Archive & ar,
			    DataModel & d,
    const unsigned int file_version
			    ){
  split_free(ar, d,  file_version); 
 }
    */
template<class Archive>
  void save(Archive & ar,const  CardData & d,const  unsigned int version)
  {
    /*
    for (int i=0;i<d.Data.size();i++){
      ar & *(d.Data.at(i));
    }
    ar & d.SubRunNumber;
    */
    //Run info
    //    ar & d.RunNumber;
    // ar & d.SubRunNumber;
    // ar & d.RunType;
    //ar & d.NumEvents;
    /*    int LastSync;
    int SequenceID;
    int StartTime;
    int CardID;
    int channels; // eg number of connected PMTs
    int *PMTID; //1D array of length channels
    int buffersize;
    int fullbuffsize; // buffersize * num channels
    uint16_t *Data; //1D 
    */
    //Board Data
   ar & d.LastSync;
   ar & d.SequenceID;
   ar & d.StartTime;
   ar & d.CardID;
   ar & d.channels;
   
   ar & d.buffersize;
   ar & d.fullbuffsize;
   
   //   int a= sizeof(d.PMTID)/sizeof(*(d.PMTID));
   //ar & a;
   for(int i=0;i<d.channels;i++){
     ar & (d.PMTID[i]);
   }
   
   //   int b= sizeof(d.Data)/sizeof(d.Data[0]);   
   //ar & b;

   for (int i=0;i<d.fullbuffsize;i++){
     ar & d.Data[i];
   }

   //   std::cout<<d.LastSync<<std::endl;
   // std::cout<<d.SequenceID<<std::endl;
   // std::cout<<d.StartTime<<std::endl;
   // std::cout<<d.CardID<<std::endl;
   // std::cout<<d.channels<<std::endl;
   //std::cout<<d.buffersize<<std::endl;
   //std::cout<<d.fullbuffsize<<std::endl;
   //   std::cout<<a<<std::endl;
   //std::cout<<b<<std::endl;
   //std::cout<<d.PMTID[0]<<std::endl;
   //std::cout<<d.Data[0]<<std::endl;

  }
 
 template<class Archive> void load(Archive & ar, CardData & d,const  unsigned int version )
 {
   /*
    for (int i=0;i<d.Data.size();i++){
      ar & *(d.Data.at(i));
    }
    ar & d.SubRunNumber;
   */
   //Run info
   //ar & d.RunNumber;
   //ar & d.SubRunNumber;
   //ar & d.RunType;
   // ar & d.NumEvents;


   //Board Data
   ar & d.LastSync;
   ar & d.SequenceID;
   ar & d.StartTime;
   ar & d.CardID;
   ar & d.channels;

   ar & d.buffersize;
   ar & d.fullbuffsize;
   
   
   //   int len;
   //ar & len;
   
   d.PMTID= new int[d.channels];
   for(int i=0;i<d.channels;i++){
     
     //int id=0;
     ar & d.PMTID[i];
     //id;

     //d.PMTID[i]=id;
     
   }
   
   //int lenb;
   // ar & lenb;
   d.Data= new uint16_t[d.fullbuffsize];
   for(int i=0;i<d.fullbuffsize;i++){

     //uint16_t data=0;
     ar & d.Data[i];

   }
   // std::cout<<d.LastSync<<std::endl;
   //std::cout<<d.SequenceID<<std::endl;
   //std::cout<<d.StartTime<<std::endl;
   //std::cout<<d.CardID<<std::endl;
   //std::cout<<d.channels<<std::endl;
   //std::cout<<d.buffersize<<std::endl;
   //std::cout<<d.fullbuffsize<<std::endl;
   // std::cout<<len<<std::endl;
   // std::cout<<lenb<<std::endl;
   //std::cout<<d.PMTID[0]<<std::endl;
   //std::cout<<d.Data[0]<<std::endl; 
 }

 } // namespace serialization
  } // namespace boost




class VMESendData: public Tool {


 public:

  VMESendData();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  zmq::socket_t *Send;
  int m_port;


};

#endif

