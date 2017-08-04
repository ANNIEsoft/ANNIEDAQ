#ifndef TEST_PLOT_H
#define TEST_PLOT_H

#include <string>
#include <iostream>

#include <Plot.h>


class TestPlot: Public Plot{
  
 public:
  
  TestPlot();
  bool Initialise(Store* variables);
  bool Execute(std::vector<CardData*> *data);
  bool Finalise();
  
  
 private:
  
  
  
};

#endif
