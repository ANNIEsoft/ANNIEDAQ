#ifndef PLOT_H
#define PLOT_H

#include <string>
#include <iostream>
#include "Store.h"
#include "CardData.h"

class Plot{
  
 public:
  
  virtual bool Initialise(Store* variables)=0;
  virtual bool Execute(std::vector<CardData*> *data)=0;
  virtual bool Finalise()=0;
  
 protected:

  
 private:
  
  
  
  
  
};

#endif
