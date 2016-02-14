#ifndef BoardReader_H
#define BoardReader_H

#include <string>
#include <iostream>
#include <sstream>
#include <time.h>

#include "Tool.h"

//#include "TTree.h"


class BoardReader: public Tool {


 public:

  BoardReader();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  int cards;
  int channels;
  int buffersize;

  /*
  int LastSync;
  int SequenceID;
  int StartTime;
  int CardID;
  int channels;
  int *PMTID;
  int buffersize;
  int fullbuffsize;
  double *Data;
  */



};


#endif
