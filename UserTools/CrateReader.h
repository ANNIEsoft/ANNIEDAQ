#ifndef CrateReader_H
#define CrateReader_H

#include <string>
#include <iostream>

#include "HardwareInterface.h"
#include "UC500ADCInterface.h"
#include "Tool.h"
#include "CardData.h"

class CrateReader: public Tool {


 public:

  CrateReader();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  //int m_crate_num;
  // int m_num_cards;


};


#endif
