#ifndef NetworkSendData_H
#define NetworkSendData_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Tool.h"

#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
//#include <boost/serialization/split_member.hpp>
#include <boost/serialization/split_free.hpp>

BOOST_SERIALIZATION_SPLIT_FREE(DataModel);

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
  void save(Archive & ar,const  DataModel & d,const  unsigned int version)
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

    
    //Board Data
   ar & d.LastSync;
   ar & d.SequenceID;
   ar & d.StartTime;
   ar & d.CardID;
   ar & d.channels;
   
   ar & d.buffersize;
   ar & d.fullbuffsize;
   
   int a=d.PMTID.size();
   ar & a;
   for(int i=0;i<d.PMTID.size();i++){
     ar & *(d.PMTID.at(i));
   }
   
   a=d.Data.size();   
   ar & a;

   for (int i=0;i<d.Data.size();i++){
     ar & *(d.Data.at(i));
   }
   
  }
 
 template<class Archive> void load(Archive & ar, DataModel & d,const  unsigned int version )
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
   
   
   int len;
   ar & len;
   
   for(int i=0;i<len;i++){
     
     int id=0;
     ar & id;

     *(d.PMTID.at(i))=id;
     
   }
   
 
   ar & len;

   for(int i=0;i<len;i++){

     double data=0.0;
     ar & data;

     *(d.Data.at(i))=data;

   }
   
 }

 } // namespace serialization
  } // namespace boost




class NetworkSendData: public Tool {


 public:

  NetworkSendData();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  zmq::socket_t *Send;
  int m_port;


};


#endif
