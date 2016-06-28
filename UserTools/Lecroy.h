#ifndef Lecroy_H
#define Lecroy_H

#include <string>
#include <iostream>
#include <ctime>

#include "Tool.h"

class Lecroy: public Tool
{


	public:

		Lecroy();
		bool Initialise(std::string configfile,DataModel &data);
		bool Execute();
		bool Finalise();


	private:


		DataModel::Channel Data;
		std::string DC;
		int perc;
		int val;
		int verb;
		int array[5];

};


#endif
