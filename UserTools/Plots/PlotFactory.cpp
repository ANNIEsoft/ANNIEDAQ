#include "../Unity.cpp"

Plot* PlotFactory(std::string plot){
  Plot* ret=0;
  
  // if (plot=="Type") ret=new Type;
  if (plot=="Type") ret=new TestPlot; 

return ret;
}

