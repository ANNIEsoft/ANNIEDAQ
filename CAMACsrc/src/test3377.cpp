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
	std::string Line;
	std::stringstream ssL;
	std::string sEmp;
	int iEmp;
	std::vector<CamacCrate*> card;
	std::vector<std::string> Lcard, Rcard;
	std::vector<int> Ncard;
	std::vector<int> list3377, list4300b;

	if (argc == 1) fin.open("config", std::fstream::in);
	else fin.open(argv[1]);

	while (getline(fin, Line))
	{
		if (Line[0] == '#') continue;
		else
		{
			ssL.str("");
			ssL.clear();
			ssL << Line;
			ssL >> sEmp;
			Lcard.push_back(sEmp);
			ssL >> iEmp;
			Ncard.push_back(iEmp);
			ssL >> sEmp;
			Rcard.push_back(sEmp);
		}
	}
	fin.close();

	for (int i = 0; i < Lcard.size(); i++)
	{
		if (Lcard.at(i) == "Lecroy3377")
		{
			card.push_back(new Lecroy3377(Ncard.at(i), 0));
			list3377.push_back(i);
		}
		else if (Lcard.at(i) == "Lecroy4300b")
		{
			card.push_back(new Lecroy4300b(Ncard.at(i), 0));
			list4300b.push_back(i);
		}
		else std::cout << "\n\nUnkown card\n\n";
	}

	std::vector<long> Data;

	card.at(0)->CommonStart();
	std::cout << "Reading registers\n";
	card.at(0)->GetRegister();
	card.at(0)->DecRegister();
	card.at(0)->PrintRegRaw();

	for (int i = 0; i < 100; i++)
	{
		std::cout << "Test " << i << std::endl;
		card.at(0)->InitTest();
		if (card.at(0)->TestEvent() == 1)
			card.at(0)->ReadFIFOall(Data);
	}

	std::cout << "Data size " << Data.size() << std::endl;
	std::ofstream fout (Rcard.at(0).c_str());
	for (int i = 0; i < Data.size(); i++)
	{
		fout << i << "\t" << std::hex << Data.at(i) << "\t";
		fout << std::dec << Data.at(i) << std::endl;
	}
	fout.close();

	return 0;
}
