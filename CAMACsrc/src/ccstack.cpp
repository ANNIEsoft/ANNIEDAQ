/*
 * Main code
 * File output not implemented yet
 *
 * Author: Tommaso Boschi
 */



#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "CamacCrate.h"
#include "Lecroy3377.h"
#include "Lecroy4300b.h"
#include "libxxusb.h"
#include "usb.h"

int main (int argc, char** argv)
{
	std::fstream fin;
	std::string stackfile;
	std::vector<std::string> vWord;

	if (argc < 2) stackfile = "Stackline";
	else stackfile = argv[1];
	
	int CamQ, CamX;

	CamacCrate *CC = new CamacCrate;
	CamacCrate *TDC = new Lecroy3377(22);
	CamacCrate *ADC = new Lecroy4300b(21);
	
	CC->LoadStack(stackfile);
	CC->ReadStack();
//	MASK in 24 bits	 321098765432109876543210
	std::string mask = "011000000000000000000000";
	CC->SetLAMmask(mask);
	CC->GlobalRegister(0x0);	

	TDC->CommonStart();
	std::cout << "TDC: Reading registers\n";

	ADC->ClearAll();
	long ctrl = 0x6000;
	ADC->WriteReg(ctrl);
	ADC->GetRegister();
	ADC->PrintRegRaw();

        CC->StartStack();

	for (int i = 0; i < 100; i++)
	{
		vWord.clear();
		CC->ReadFIFO(vWord);
		for (int j = 0; j < vWord.size(); j++)
			std::cout << i+j << "\t" << vWord.at(i) << std::endl;
	}

	return 0;
}
