#include <iostream>
#include <fstream>
#include <string>
#include <sstring>
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
	std::string sEmp0;
	int iEmp;
	std::vector<CamacCrate*> card;
	std::vector<std::string> Lcard, Rcard;
	std::vector<int> Ncard;

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
			Lcard.push_back(emp);
			ssL >> iEmp;
			Ncard.push_back(iEmp);
			ssL >> sEmp;
			Rcard.push_back(emp);
		}
	}
	fin.close();

	int nL3377, nL4300b;
	card.resize(Lcard.size());
	for (int i = 0; i < card.size(); i++)
	{
		if (Lcard.at(i) == "Lecroy3377")
			card.at(i) = new Lecroy3377(Ncard.at(i));
		else if (Lcard.at(i) == "Lecroy4300b")
			card.at(i) = new Lecroy4300b(Ncard.at(i));
		else std::cout << "\n\nUnkown card\n\n";
	}

	for (int i = 0; i < card.size(); i++)
	{
		std::cout << "..Analysing card in slot " << Ncard.at(i) << std::endl;
		card.at(i)->GetRegister();
		card.at(i)->PrintRegister();
	}

	int *ret = new int[Lcard.size()];
	long pureData;
	std::vector<std::vector<long> > vData;
	vData.resize(Lcard.size()];

	do
	{
		if (card0->TestEvent() == 1)
			ret0 = card0->ReadFIFOall(Data3377);
		if (card1->TestLAM() == 1)
			ret1 = card1->GetData(Data4300b, SAD4300b);
	}
	while ((ret0 < 0) || (ret1 < 0));

	for (int i = 0; i < Data3377.size(); i++)
		std::cout << Data3377.at(i) << std::endl;
	for (int i = 0; i < Data4300b.size(); i++)
	{
		std::cout << Data4300b.at(i) << std::endl;
		std::cout << SAD4300b.at(i) << std::endl;
	}

	return 0;
}
