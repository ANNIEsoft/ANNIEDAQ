#ifndef ORIGINAL_PLOTS_H
#define ORIGINAL_PLOTS_H

#include <string>
#include <iostream>

#include <Plot.h>

#include <pqxx/pqxx>

#include "TCanvas.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TGraph2D.h"
#include "TLegend.h"
#include "TF1.h"

struct PMT{

  int gx,gy,gz,card,channel;

};

class OriginalPlots: public Plot {
  
 public:
  
  OriginalPlots();
  bool Initialise(Store* variables);
  bool Execute(std::vector<CardData*> *data);
  bool Finalise();
  
  
 private:

  TCanvas* c1;
  std::vector<PMT> PMTInfo;
  std::string outpath;
  bool init;  
  std::map<int,std::vector<TH1F> > PedTime;
  std::map<int,std::vector<TH1F> > PedRMSTime;
  std::vector<TH1F> rates;
  std::vector<TH1F> averagesize;

  std::vector<TH1I> tfreqplots;
  std::map<int,std::vector<std::vector<float > > > pedpars;
  
  
};

#endif
