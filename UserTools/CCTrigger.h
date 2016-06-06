#ifndef CCTrigger_H
#define CCTrigger_H

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "Tool.h"

class CCTrigger: public Tool
{


	public:

		CCTrigger();
		bool Initialise(std::string configfile,DataModel &data);
		bool Execute();
		bool Finalise();


	private:
		
		inline CamacCrate* Create(std::string cardname, int cardslot);

		CamacCrate* CC;
		std::vector<std::string> Lcard;
		std::vector<int> Ncard;
	
		int verb;
		std::string configcc;
		bool trg_mode;
		int trg_pos;

};


#endif
