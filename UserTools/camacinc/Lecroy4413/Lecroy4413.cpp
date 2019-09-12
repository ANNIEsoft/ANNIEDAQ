/* vim:set noexpandtab tabstop=4 wrap */
#include "CamacCrate.h"
#include "Lecroy4413.h"

LeCroy4413::LeCroy4413(int NSlot, std::string config, int i) : CamacCrate(i)//Subclass constructor, n of Slot given
{
  int gotQresp;
  Slot.push_back(NSlot);
  ID = Slot.size()-1;
  SetConfig(config);
  //std::cout<<"constructor setting remote mode"<<std::endl;
  SetRemoteMode(1);
  //std::cout<<"constructor setting threshold mode "<<((thresholdmode) ? "FP" : "programmed")<<std::endl;
  // IT IS IMPORTANT WE DO THIS BEFORE WRITING THE THRESHOLD VALUE, AS THE MEMBER VARIABLE SET BY 'SetConfig' WILL GET OVERWRITTEN WHEN WRITING THRESHOLD VALUE
  if(thresholdmode==1) EnableFPthreshold();
  else EnableProgrammedThreshold();
  std::cout<<"constructor writing threshold value "<<threshold<<std::endl;
  WriteThresholdValue(threshold);
  int set=0;
  ReadThreshold(set);
  std::cout<<"threshold value set to"<<set<<std::endl;
  

  //std::cout << "l slot size " << Slot.size() << std::endl;
  //for (int i = 0; i < Slot.size(); ++i)
  //std::cout << "ll " << Slot.at(i) << std::endl;
  //std::cout<<"l7"<<std::endl; 
}

int LeCroy4413::ReadThreshold(int& thresholdin) //F(1)·A(0) Read programmed discriminator threshold, F(1)·A(0)
{
  // NOTE: Could not confirm if this reads the threshold set with the FP potentiometer. 
  // On the card used for testing, the FP screw made no change to the threshold value.
  
  //std::cout<<"reading threshold "<<std::endl;
  int Data = 0;
  int Q = 0, X = 0;
  
  int ret = READ(1, 0, Data, Q, X);
  //std::cout<<"ReadThreshold response: ret="<<ret<<", Q="<<Q<<", X="<<X<<std::endl; // FIXME debug
  if(ret < 0 || Q==0){ std::cerr<<"failed to read threshold, aborting"<<std::endl; return (ret < 0 ) ? ret : Q; }
  if ((ret >= 0) && thresholdin!=999){
    // actual threshold value is only in bits 0-9, mask other bits
    std::bitset<16> DataAsBitset(Data);
    //thresholdmode = DataAsBitset.test(10);   // Appears the representative value of this bit is NOT returned, so masking is also probably not necessary, but safer.
    DataAsBitset &= ValueMask;
    thresholdin = DataAsBitset.to_ulong();     // TODO check signage?
    //std::cout<<"ReadThreshold finds the returned threshold is "<<Data<<" (thresholdin set to "<<thresholdin<<")"<<std::endl;
    threshold = thresholdin;
  }
  return (ret < 0 ) ? ret : Q;
}

int LeCroy4413::ReadThreshold() // convenience function
{
  int dummy=999;
  int ret = ReadThreshold(dummy);
  return ret;
}

int LeCroy4413::WriteThresholdValue(int thresholdin) //F(17)·A(0) bits 0-9 
{
  // WriteThresholdValue behaviour is weird with threshold set to FP mode - the write succeeds, and the value is even set(?) breifly...
  // e.g. after WriteThresholdValue(50), consequential executions of ReadThresholdValue return 48, then 32, then 23, then 15,
  // eventually settling to 15...
  if(thresholdmode==1){
    std::cerr<<"Lecroy4413: Cannot write threshold value when threshold mode is set to front-panel potentiometer"<<std::endl
	     <<"Call `EnableProgrammedThreshold()` first."<<std::endl;
    return -1;
  }
  
  // MINIMUM THRESHOLD VALUE IS 15MV. SET CALLS WITH VALUES BELOW THIS WILL BE COERCED!
  if(thresholdin<15) std::cout<<"Lecroy4413::WriteThresholdValue warning! Requested threshold is below the minimum of 15mV, it will be coerced to the valid range."<<std::endl;
  
  int Data = thresholdin;
  int Q = 0, X = 0;
  
  // coerce to a valid range
  if(thresholdin>1024){
    std::cout<<"Lecroy4413::WriteThresholdValue warning! Requested threshold exceeds maximum value of 1023mV, it will be coerced to the valid range."<<std::endl;
    std::bitset<16> DataAsBitset(static_cast<uint16_t>(thresholdin));
    // ensure we don't inadvertantly change the thresholdmode by setting a value outside valid range
    DataAsBitset &= ValueMask;
    //DataAsBitset.set(10,thresholdmode);  // we could also set this to prevent changing the FP mode, but we break early if FP mode is potentiometer anyway.
    Data = DataAsBitset.to_ulong();
  }
  
  int ret = WRITE(17, 0, Data, Q, X);
  //std::cout<<"WriteThreshold("<<Data<<"["<<std::bitset<16>(Data)<<"]) returned ret="<<ret<<", Q="<<Q<<", X="<<X<<std::endl;
  if((not (ret<0)) && Q==1) threshold = thresholdin;
  return (ret < 0 ) ? ret : Q;
}

int LeCroy4413::EnableFPthreshold() //F(17)·A(0) Set threshold bit 10 to 1 (enable threshold control via front panel potentiometer)
{
  // we want to set just one bit: first read the threshold register (or used the stored member...)
  int Data = 0;
  int Q = 0, X = 0;
  int ret = READ(1, 0, Data, Q, X);
  //std::cout<<"ReadThreshold response: ret="<<ret<<", Q="<<Q<<", X="<<X<<std::endl; // debug
  if(ret < 0 || Q==0){ std::cerr<<"failed to read threshold, aborting"<<std::endl; return (ret < 0 ) ? ret : Q; }
  
  // now modify the necessary bit
  std::bitset<16> DataAsBitset(static_cast<uint16_t>(Data));
  DataAsBitset.set(10,1);
  Data = static_cast<int>(DataAsBitset.to_ulong());
  ret = WRITE(17, 0, Data, Q, X);
  //std::cout<<"WriteThresholdMode response: ret="<<ret<<", Q="<<Q<<", X="<<X<<std::endl; // debug
  if((not (ret<0)) && Q==1) thresholdmode = 1;
  
  SetRemoteMode(RemoteMode);  // XXX card stops responding for arbitrary time after EnableFPthreshold is called???
  
  return (ret < 0 ) ? ret : Q;
}

int LeCroy4413::EnableProgrammedThreshold() //F(17)·A(0) Set threshold bit 10 to 0 (enable threshold control via programmed register)
{
  // we want to set just one bit: first read the threshold register
  int Data = 0;
  int Q = 0, X = 0;
  int ret = READ(1, 0, Data, Q, X);
  //std::cout<<"ReadThreshold response: ret="<<ret<<", Q="<<Q<<", X="<<X<<std::endl; // debug
  if(ret < 0 || Q==0){ std::cerr<<"failed to read threshold, aborting"<<std::endl; return (ret < 0 ) ? ret : Q; }
  
  // now modify the necessary bit
  std::bitset<16> DataAsBitset(static_cast<uint16_t>(Data));
  DataAsBitset.set(10,0);
  Data = static_cast<int>(DataAsBitset.to_ulong());
  ret = WRITE(17, 0, Data, Q, X);
  //std::cout<<"WriteThresholdMode response: ret="<<ret<<", Q="<<Q<<", X="<<X<<std::endl; // debug
  if((not (ret<0)) && Q==1) thresholdmode = 0;
  
  return (ret < 0 ) ? ret : Q;
}

int LeCroy4413::ReadChannelMask(int& channelmaskin) //F(0)·A(0) Read register containing disabled channels
{
  int Data = 0;
  int Q = 0, X = 0;
  int ret = READ(0, 0, Data, Q, X);
  //std::cout<<"ReadChannelMask: ret="<<ret<<", Q="<<Q<<", X="<<X<<std::endl;
  
  if((not (ret<0)) && Q==1){
    channelmaskin = Data;
    channelmask=std::bitset<16>(static_cast<uint16_t>(Data));
  }
  return (ret < 0 ) ? ret : Q;
}

int LeCroy4413::ReadChannelMask() // convenience function for testing remote response
{
  int dummy=0;
  int ret = ReadChannelMask(dummy);
  return ret;
}

int LeCroy4413::WriteChannelMask(int& channelmaskin) //F(16)·A(0) Write register containing disabled channels
{
  int Q = 0, X = 0;
  int ret = WRITE(16, 0, channelmaskin, Q, X);
  
  if((not (ret<0)) && Q==1){
    channelmask=std::bitset<16>(static_cast<uint16_t>(channelmaskin));
  }
  return (ret < 0 ) ? ret : Q;
}

int LeCroy4413::TestInit() //F(25)·A(0) Generate a 3ns output pulse on all non-masked channels
{
  int Data=1;
  int Q = 0, X = 0;
  int ret = READ(25, 0, Data, Q, X);
  
  return (ret < 0 ) ? ret : Q;
}

int LeCroy4413::SetRemoteMode(int modein, int timeout_ms) //F(24/26)·A(0) 24=local, 26=remote. input: local=0, remote=1
{
  int Data=1;
  int Q = 0, X = 0;
  int ret;
  
  // this doesn't work reliably. Use a loop with a timeout to keep trying until we get access.
  unsigned long start_ms = clock()/(CLOCKS_PER_SEC/1000);
  unsigned long curr_ms = start_ms;
  bool timedout=true;
  //std::cout<<"start_ms="<<start_ms<<std::endl;
  while ((curr_ms-start_ms)<timeout_ms){
    
    if(modein==0) ret = READ(24, 0, Data, Q, X);        // local mode
    else if(modein==1) ret = READ(26, 0, Data, Q, X);   // remote mode
    else {
      std::cerr<<"LeCroy4413::SetRemoteMode unrecognised mode "<<modein<<": 0=local, 1=remote"<<std::endl;
      ret = -1;
    }
    if((not (ret<0))){  // the return Q represents the PREVIOUS remote mode; if it was remote, Q=1, if it was local, Q=0
      RemoteMode = modein;
    }
    //std::cout<<"SetRemoteMode("<<modein<<") got ret="<<ret<<", Q="<<Q<<", X="<<X<<std::endl;
    
    int gotQresp = ReadChannelMask();
    //std::cout<<"mode is now "<<((gotQresp==1) ? "remote" : "local")<<std::endl;
    if(ret<0){ std::cerr<<"ReadChannelMask error: ret="<<ret<<std::endl; timedout=false; break; }
    else if(gotQresp || modein==0){ timedout=false; break; }
    else curr_ms = clock()/(CLOCKS_PER_SEC/1000);
    //std::cout<<"clock()="<<clock()<<", curr_ms="<<curr_ms<<", elapsed time: "<<(curr_ms-start_ms)<<std::endl;
    usleep(100);
  }
  //std::cout<<"end_ms="<<(clock()/(CLOCKS_PER_SEC/1000))<<": total time elapsed = "<<((clock()/(CLOCKS_PER_SEC/1000))-start_ms)<<std::endl;
  if(timedout) std::cerr << "LeCroy4413::SetRemoteMode("<<modein<<") timed out in slot " << GetSlot() << std::endl;
  
  
  return (ret < 0 ) ? ret : Q;
}

int LeCroy4413::GetRemoteMode() // No camac function: return member variable. output: local=0, remote=1
{
  // could be more robust to run a query which should return a valid Q response in remote mode? 
  // but this could also fail for other reasons...
  int gotQresp = ReadChannelMask();
  if(gotQresp==1) RemoteMode=1;
  else RemoteMode=0;
  return RemoteMode;
}

//////////////////////
// common CAMAC commands

int LeCroy4413::READ(int F, int A, int &Data, int &Q, int &X)//Generic READ
{
  long lData;
  int ret = CamacCrate::READ(GetID(), A, F, lData, Q, X);
  Data = lData;
  //std::cout << "GetID (): " << GetID() << "\n lData = " << lData << std::endl;
  //usleep(5000);
  return ret;
}

int LeCroy4413::WRITE(int F, int A, int &Data, int &Q, int &X)//Gneric WRITE
{
  long lData = long(Data);
  int ret = CamacCrate::WRITE(GetID(), A, F, lData, Q, X);
  //usleep(5000);
  return ret;
}

int LeCroy4413::GetID()//Return ID of module
{
  return ID;
}

int LeCroy4413::GetSlot()//Return n of Slot of module
{
  return Slot.at(GetID());
}

//int LeCroy4413::EnLAM() //Enable LAM.
//{
//int Data = 0;
//int Q = 0, X = 0;
//int ret = READ(0, 26, Data, Q, X);
//return ret;
//}

//int LeCroy4413::TestLAM() //Test LAM.
//{
//int Data = 0;
//int Q = 0, X = 0;
//int ret = READ(0, 8, Data, Q, X);
//return Q;
//}

//int LeCroy4413::ClearLAM() //Clear LAM.
//{
//int Data = 0;
//int Q = 0, X = 0;
//int ret = READ(0, 10, Data, Q, X);
//return ret;
//}

//int LeCroy4413::DisLAM() //Disable LAM.
//{
//int Data = 0;
//int Q = 0, X = 0;
//int ret = READ(0, 24, Data, Q, X);
//return ret;
//}

// not really applicable, but set any internal configurations with settings from file
void LeCroy4413::SetConfig(std::string config)
{
  std::ifstream fin (config.c_str());
  std::string Line;
  std::stringstream ssL;
  
  std::string sEmp;
  int iEmp;
  //std::cout<<"conf 1"<<std::endl;
  bool maskSettings=false;
  while (getline(fin, Line))
    {
      //std::cout<<"conf 2"<<std::endl;
      if (Line.empty()) continue;
      if (Line[0] == '#') continue;
      if (Line.find("StartChannelMask") != std::string::npos) maskSettings = true;
      if (Line.find("EndChannelMask") != std::string::npos) maskSettings = false;
      else
	{
	  Line.erase(Line.begin()+Line.find('#'), Line.end());
	  ssL.str("");
	  ssL.clear();
	  ssL << Line;
	  if (ssL.str() != "")
	    {
	      ssL >> sEmp >> iEmp;
	      // Read internal parameters from file here... 
	      if (sEmp == "Threshold") threshold = iEmp;
	      else if (sEmp == "RemoteMode") RemoteMode = iEmp;
	      else if (sEmp == "ThresholdMode") thresholdmode = iEmp;
	      else if(maskSettings){
		int chan;
		std::stringstream tmp(sEmp);
		tmp>>sEmp;
		//int chan = std::stoi(sEmp);
		if (chan >= 0 && chan < 16){
		  channelmask.set(chan,iEmp);
		} else {
		  std::cerr<<"WARNING: Lecroy4413 ignoring out of range channel "<<chan<<" in ChannelMask configuration"<<std::endl;
		}
	      }
	      else std::cerr<<"WARNING: Lecroy4413 ignoring unknown configuration option \""<<sEmp<<"\""<<std::endl;
	    }
	}
    }
  fin.close();
}
