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

	for (int i = 0; i < card.size(); i++)
	{
		std::cout << "..Analysing card in slot " << Ncard.at(i) << std::endl;
		card.at(i)->GetRegister();
		card.at(i)->PrintRegister();
	}

	int *ret = new int[Lcard.size()];
	std::vector<std::vector<long> > vData;
	vData.resize(Lcard.size()+list4300b.size());

	int k;
	bool check = true, r;
	do
	{
		for (int i = 0; i < list3377.size(); i++)
		{
			std::cout << "c0.0" << std::endl;
			k = list3377.at(i);
			std::cout << "c0.1" << std::endl;
		//	if (card.at(k)->TestEvent() == 1)
			std::cout << "c0.2" << std::endl;
				ret[k] = card.at(k)->ReadFIFOall(vData.at(k));
		}
		for (int i = 0; i < list4300b.size(); i++)
		{
			std::cout << "c1.0" << std::endl;
			k = list4300b.at(i);
			std::cout << "c1.1" << std::endl;
		//	if (card.at(k)->TestLAM() == 1)
			std::cout << "c1.2" << std::endl;
				ret[k] = card.at(k)->GetData(vData.at(k), vData.at(k+list4300b.size()));
		}
		for (int i = 0; i < Lcard.size(); i++)
		{
			std::cout << "c2" << std::endl;
			r = ret[i] < 0;
			check = check && !r;
			std::cout << "check " << check << std::endl;
		}
	}
	while (check);

	for (int i = 0; i < vData.size(); i++)
		std::cout << i % Lcard.size() << "\t" << vData.at(i).size() << std::endl;

	return 0;
}
