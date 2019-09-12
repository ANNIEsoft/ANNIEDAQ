/* vim:set noexpandtab tabstop=4 wrap */
/*
 * Jorway 85A Scaler CAMAC module class
 *
 * Author: Marcus O'Flaherty, based on code by Tommaso Boschi
 */

#ifndef LeCroy4413_H
#define LeCroy4413_H

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <ctime>
#include <bitset>
#include <stdint.h>

#include "libxxusb.h"
#include "CamacCrate.h"

namespace {
  std::bitset<16> ValueMask(std::string("0000001111111111"));
}

class LeCroy4413 : public CamacCrate
{
 public:
  /*LeCroy4413(int NSlot, int i = 0) : CamacCrate(i)//Subclass constructor, n of Slot given
    {
    Slot.push_back(NSlot);
    ID = Slot.size()-1;
    }
  */

  // Dataway Controls
  // ================
  // 
  // Initialize: Z() Clears mask register and Test Mode at S2 time and **sets module into Local mode**
  // Inhibit: I disables all outputs in Remote mode.
  // 
  // ==================
  // N-F(1)-A(O):                 Read threshold setting.
  // N-F(O)-A(O):                 Read mask register. Q response in Remote mode.
  // N-F(16)-A(O):                Writes mask register pattern (W1-W16). X + Q response in Remote mode.
  // N-F(17)-A(O):                Writes threshold setting register; 11 bits, 10 bits of data (W1-W10)
  //                              providing 1 mV resolution are used to program common threshold and 1 bit (W11)
  //                              to set the manual threshold value (front-panel screwdriver control) to the
  //                              threshold register. In this case, the data presented on W1-W10 is ignored.
  //                              Q response in Remote mode.
  // N-F(25)-A(O):                Test function. Only channels not masked off by F(16) are triggered by
  //                              internally generated 3 nsec wide pulse. Q response in Remote mode.
  // N-F(24)-A(O):                Set Local mode.
  // N-F(26)-A(O):                Set Remote mode.
  // 
  // X: X=1 response is generated for every valid F,N,A.
  // Q: Q=1 response is generated if the valid command can be executed.
  // 
  // ==================
  ///////////////////////////

  LeCroy4413(int NSlot, std::string config, int i = 0);// : CamacCrate(i);//Constructor !

  int ReadThreshold(int& threshold);//F(1)·A(0)
  int ReadThreshold();//F(1)·A(0)
  int WriteThresholdValue(int thresholdin);//F(17)·A(0) bits 0-9
  int EnableFPthreshold();//F(17)·A(0) bit 10 ->1  (invoke/same as RemoteMode?)
  int EnableProgrammedThreshold();//F(17)·A(0) bit 10 ->0  (invoke/same as RemoteMode?)
  int ReadChannelMask(int& channelmaskin);//F(0)·A(0)
  int ReadChannelMask();//F(0)·A(0)
  int WriteChannelMask(int& channelmaskin);//F(16)·A(0)
  int TestInit();//F(25)·A(0)
  int SetRemoteMode(int modein, int timeout_ms=5000);//F(24/26)·A(0) 24=local, 26=remote. INPUT: local=0, remote=1
  int GetRemoteMode();//return member variable, or test for response from e.g. ReadChannelMask. OUTPUT: local=0, remote=1
  
  // inherited functions
  int READ(int F, int A, int &Data, int &Q, int &X);//F(x)·A(y)
  int WRITE(int F, int A, int &Data, int &Q, int &X);//F(x)·A(y)
  
  int GetID();//Return ID
  int GetSlot();//Return Slot
  
  void SetConfig(std::string config);//Set register from configfile
  
 private:
  
  std::bitset<16> channelmask;  // disabled channels
  int threshold;                // mV, 10 bits
  int RemoteMode;               // local / remote: controls whether the card responds to commands... also threshold mode?
  int thresholdmode;            // FP / programmed: controls whether the card threshold is read from FP potentiometer or internal register
  int ID;                       //
  
};

#endif
