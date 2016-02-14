#ifndef HARDWAREINTERFACE_H
#define HARDWAREINTERFACE_H

#include "CardData.h"

class HardwareInterface{

 public:

  virtual bool Triggered() = 0;
  virtual bool Triggered(int card)=0;
  virtual int NumberOfCards() =0;
  virtual CardData* GetCardData(int card) = 0;
  virtual bool EnableTrigger()=0;
  // not sure if you necessarily need initialisation and finalisation for the cards but if not delete below
 
  virtual bool Initialise() = 0;
  virtual bool Finalise() = 0;  
  
};

#endif
