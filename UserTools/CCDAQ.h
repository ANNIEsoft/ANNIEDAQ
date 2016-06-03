#ifndef CCDAQ_H
#define CCDAQ_H

#include <string>
#include <iostream>

#include "Tool.h"

#include "CamacCrate.h"
#include "Lecroy3377.h"
#include "Lecroy4300b.h"
#include "libxxusb.h"
#include "usb.h"


class CCDAQ: public Tool {


	public:

		CCDAQ();
		bool Initialise(std::string configfile,DataModel &data);
		bool Execute();
		bool Finalise();


	private:

		void StackBuilder (std::string configname);
		std::vector<int> vStack;

		std::string configcc, stackfile;
		std::vector<CamacCrate*> Card;
		std::vector<std::string> Lcard;
		std::vector<int> Ncard, vWord;
		std::vector<int> L3377, L4300;
};


#endif
