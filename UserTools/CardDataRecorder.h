#ifndef CardDataRecorder_H
#define CardDataRecorder_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "Tool.h"
#include <zmq.hpp>

#include "TFile.h"
#include "TTree.h"
#include "TThread.h"

#include <boost/progress.hpp>

struct card_root_thread_args{
  
  //root_thread_args(std::string outfile, TTree* intree){
  // OutFile=outfile;
  // tree=intree;
  //}

  card_root_thread_args(std::string outfile, std::string infilename, zmq::context_t* incontext, int inTFileTTreeCap, long &infilepart){
    
    OutFile=outfile;
    context=incontext;
    TFileTTreeCap=inTFileTTreeCap;
    filepart=&infilepart;
    filename=infilename;
  }
  
  zmq::context_t* context;
  std::string OutFile;
  int TFileTTreeCap;
  long *filepart;
  std::string filename;

};



class CardDataRecorder: public Tool {


 public:

  CardDataRecorder();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  static void *FlowManager(void* arg);
  static void *RootWriter(void* arg);

  zmq::socket_t *Isend;

  //  pthread_t thread[2];
  TThread *h1;
  card_root_thread_args *args;

  std::string OutputPath;
  std::string OutFile; //can be map
 
  int m_TTreeEventCap;
  long m_treenum; // can make map in future if more trees needed
  int m_TFileTTreeCap;
  long m_filepart;
 


  CardData m_card;
  /*int LastSync;
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
