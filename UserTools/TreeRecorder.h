#ifndef TreeRecorder_H
#define TreeRecorder_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "Tool.h"
#include <zmq.hpp>

#include "TFile.h"
#include "TTree.h"
#include "TThread.h"
#include "TRandom3.h"

#include <boost/progress.hpp>

struct card_root_thread_args
{
	
	//root_thread_args(std::string outfile, TTree* intree){
	// OutFile=outfile;
	// tree=intree;
	//}

//	card_root_thread_args(std::string outfile, zmq::context_t* incontext, int inFileCap, int &inFileCount)
	card_root_thread_args(std::string outfile, zmq::context_t* incontext, int &inFileCount)
	{
		
		out = outfile;
		context = incontext;
//		cap = inFileCap;
		part = &inFileCount;
	}
	
	zmq::context_t* context;
	std::string out;
//	int cap;
	int *part;
};



class TreeRecorder: public Tool
{
	public:

		TreeRecorder();
		bool Initialise(std::string configfile,DataModel &data);
		bool Execute();
		bool Finalise();

	private:
	
		static void *RootWriter(void* arg);
	
		zmq::socket_t *Isend;

//		pthread_t thread[2];
		TThread *h1;
		TTree *tree;
		card_root_thread_args *args;
	
		std::string OutPath, OutName, configcc;
 
		int TreeCap, FileCap, FileCount, Entries;
		int ThreadCount;
 
		std::vector<std::string> Lcard;
		std::vector<int> Ncard;

		unsigned int TOutN, AOutN, OutN, Trigger;
		unsigned int TSlot[512], TChannel[512];
		unsigned int ASlot[512], AChannel[512];
		int TDC[512], ADC[512];

		std::vector<DataModel::Channel>::iterator is;	//Iterator over the Cards, a vec of Channels map 
		std::map<int, int>::iterator it;		//Iterator over the Channel of a Card

};

#endif
