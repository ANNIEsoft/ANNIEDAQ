#ifndef PLOT_H
#define PLOT_H

#include <string>
#include <iostream>
#include "Store.h"
#include "CardData.h"

struct PMT{

  int gx,gy,gz,card,channel;
  float PEDmean,PEDsigma;

};

class Plot{
  
 public:
  
  virtual bool Initialise(Store* variables, std::map<int,std::map<int,PMT> >* PMTInfo)=0;
  virtual bool Execute(std::vector<CardData*> *data)=0;
  virtual bool Finalise()=0;
  
 protected:

  
 private:
  
  
  
  
  
};

#endif
