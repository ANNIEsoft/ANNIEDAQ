#ifndef CCUSB_H
#define CCUSB_H

#include <string>
#include <iostream>
#include <ctime>

#include "Tool.h"

#include "CamacCrate.h"
#include "Lecroy3377.h"
#include "Lecroy4300b.h"
#include "libxxusb.h"
#include "usb.h"


class CCUSB: public Tool {


	public:

		CCUSB();
		bool Initialise(std::string configfile,DataModel &data);
		bool Execute();
		bool Finalise();


	private:

		void StackBuilder (std::string configname);
		std::vector<int> vStack;

		std::string configcc, stackfile;
		std::vector<CamacCrate*> Card;
		std::vector<std::string> Lcard;
//		std::vector<std::string> Lcard(1, "CamacCrate");
		std::vector<int> Ncard;
//		std::vector<int> Ncard(1, 0);
		std::vector<int> L3, L4;
		std::map<int, int> Data;

		std::string CN;		//Card Name
};


#endif
