#ifndef MRDDATA_H
#define MRDDATA_H

#include <iostream>
#include <vector>
#include <string>

#include "CamacCrate.h"

#include "boost/date_time/gregorian/gregorian.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/chrono.hpp>

class MRDData{
  
 public:
  
  struct Channel          //Bunch of Channel makes Card                                         
  {
    std::map<int, int> ch;
  };
  
  struct Card             //Array of Card make Module                                           
  {
    std::vector<Channel> Num;
    std::vector<int> Slot;
    std::vector<int> Crate;
  };
  
  struct Module           //One Struct to rule them all, One Struct to find them, One Struct to	bring them all and in the darkness bind them                                                                 
  {
    std::map<std::string, Card> Data;                       //Data                        
    std::map<std::string, std::vector<CamacCrate*> > CC;    //Camac module class          
  };
  
  
  MRDData();
  
  
  Module List;                                    //Data Model for Lecroys                      
  
  bool TRG;               //Trigger?      Activate other tools                                  
  int trg_mode;           //Trigger mode  0 for real, 1 for soft, 2 for test                    
  
  boost::posix_time::ptime begin, end;            //Measuring time                              
  boost::posix_time::ptime LocalTime;             //Get local time      

  unsigned long triggernum;
  
  
 private:
  
};

#endif
