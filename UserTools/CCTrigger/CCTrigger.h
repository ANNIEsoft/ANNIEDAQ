#ifndef CCTrigger_H
#define CCTrigger_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>

#include "Tool.h"
#include "Lecroy3377.h"
#include "Lecroy4300b.h"
#include "Lecroy4413.h"


class CCTrigger: public Tool
{


	public:

		CCTrigger();
		bool Initialise(std::string configfile,DataModel &data);
		bool Execute();
		bool Finalise();


	private:
		
  inline CamacCrate* Create(std::string cardname, std::string config, int cardslot, int crate);

		CamacCrate* CC;
		std::vector<std::string> Lcard, Ccard;
  std::vector<int> Ncard, Ncrate ;
	
		int verb;
		int perc;
		std::string configcc;
		bool trg_mode;
		int trg_pos;

		std::map<std::string, std::vector<CamacCrate*> >::iterator iL;	//Iterates over Module.CC
		std::vector<CamacCrate*>::iterator iC;				//Iterates over Module.CC<vector>
		zmq::socket_t* TriggerSend;
		zmq::pollitem_t items[1];
		std::vector<LeCroy4413*> disc;
		int threshold;
		unsigned long reftriggernum;
};


#endif
