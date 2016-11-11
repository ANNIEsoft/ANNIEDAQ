#ifndef CCTrigger_H
#define CCTrigger_H

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "Tool.h"
#include "Lecroy3377.h"
#include "Lecroy4300b.h"

class CCTrigger: public Tool
{


	public:

		CCTrigger();
		bool Initialise(std::string configfile,DataModel &data);
		bool Execute();
		bool Finalise();


	private:
		
		inline CamacCrate* Create(std::string cardname, std::string config, int cardslot);

		CamacCrate* CC;
		std::vector<std::string> Lcard, Ccard;
		std::vector<int> Ncard;
	
		int verb;
		int perc;
		std::string configcc;
		bool trg_mode;
		int trg_pos;

		std::map<std::string, std::vector<CamacCrate*> >::iterator iL;	//Iterates over Module.CC
		std::vector<CamacCrate*>::iterator iC;				//Iterates over Module.CC<vector>

};


#endif
