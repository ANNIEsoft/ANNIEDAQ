#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <map>
#include <string>
#include <vector>

#include "TTree.h"

#include "Store.h"
#include "Logging.h"
#include "CardData.h"
#include "CamacCrate.h"

#include <zmq.hpp>



class DataModel{



	public:
	  
		struct Channel		//Bunch of Channel makes Card
		{
			std::map<int, int> ch;
		};
		
		struct Card		//Array of Card make Module
		{
			std::vector<Channel> Num;
			std::vector<int> Slot;
		};
		
		struct Module		//One Struct to rule them all, One Struct to find them, One Struct to bring them all and in the darkness bind them
		{
			std::map<std::string, Card> Data;			//Data
			std::map<std::string, std::vector<CamacCrate*> > CC;	//Camac module class
		};
	
		DataModel();
		zmq::context_t* context;
	
		TTree* GetTTree(std::string name);
		void AddTTree(std::string name,TTree *tree);
		void DeleteTTree(std::string name);
		
		Store vars;
		Logging *Log;
	
		//Run info
		long RunNumber;
		long SubRunNumber;
		long NumEvents;
		bool triggered;
		int RunType;
	
	//	Board Data
		std::vector<int> LastSync;
		std::vector<int> SequenceID;
		std::vector<int> StartTime;
		std::vector<int> CardID;
		std::vector<int> channels;
		std::vector<int*> PMTID;
		std::vector<int> buffersize;
		std::vector<int> fullbuffsize;
		std::vector<double*> Data;
	
		std::vector<CardData*> carddata; 
	 
	//	std::vector<std::map<int, int> > L3DM;	//Data Model for L3377
	//	std::vector<std::map<int, int> > L4DM;	//Data Model for L4300
	
		Module List;					//Data Model for Lecroys
	
		bool TRG;			//Trigger boolean
	
		boost::posix_time::ptime begin, end;		//Measuring real time


	private:
  
		std::map<std::string,TTree*> m_trees;	//TTree map 
  
  
};



#endif
