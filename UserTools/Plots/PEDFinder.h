#ifndef PEDFINDER_H
#define PEDFINDER_H

#include <string>
#include <iostream>

#include <Plot.h>

#include "TCanvas.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TGraph2D.h"
#include "TLegend.h"
#include "TF1.h"

class PedFinder: public Plot {
  
 public:
  
  PedFinder();
  bool Initialise(Store* variables, std::map<int, std::map<int,PMT> >* PMInfo);
  bool Execute(std::vector<CardData*> *data);
  bool Finalise();
  
  
 private:

  std::map<int,std::map<int,PMT> >* PMTinfo;
  std::map<int,std::map<int,std::deque<float> > > PEDmeam;
  std::map<int,std::map<int,std::deque<float> > > PEDsigma;
  std::map<int,std::map<int,TH1F> > PEDhist; 

  //std::map<int,std::vector<TH1F> > PedTime;
  //std::map<int,std::vector<TH1F> > PedRMSTime;
  // std::vector<TH1F> rates;
  // std::vector<TH1F> averagesize;

  //std::vector<TH1I> tfreqplots;
  //std::map<int,std::vector<std::vector<float > > > pedpars;

  TCanvas* c1;
   
};

#endif
