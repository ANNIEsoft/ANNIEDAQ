#ifndef CrateReaderDummy_H
#define CrateReaderDummy_H

#include <string>
#include <iostream>

#include "HardwareInterface.h"
#include "UC500ADCInterface.h"
#include "Tool.h"
#include "CardData.h"

class CrateReaderDummy: public Tool {


 public:

  CrateReaderDummy();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  //int m_crate_num;
  // int m_num_cards;


};


#endif
