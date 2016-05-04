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

int main (int argc, char** argv)
{
	std::fstream fin, fout;
	std::string Line;
	std::stringstream ssL;
	std::string sEmp;
	int N, A, F, CB, max, cmd;
	std::vector<int> Stack;

	if (argc < 2) fin.open("Stackconfig", std::fstream::in);
	else fin.open(argv[1]);

	while (getline(fin, Line))
	{
		if (Line[0] == '#') continue;
		else
		{
			ssL.str("");
			ssL.clear();
			ssL << Line;
			ssL >> N;
			ssL >> A;
			ssL >> F;
			ssL >> CB;
			ssL >> max;
			cmd = (CB << 15) + (N << 9) + (A << 5) + F;
			Stack.push_back(cmd);
			if (CB)
			{
				cmd = 1 << 4;
				Stack.push_back(cmd);
				Stack.push_back(16);
			}
		}
	}
	fin.close();

	if (argc < 3) fout.open("Stackline", std::fstream::out);
	else fout.open(argv[2]);

	fout << std::hex << Stack.size() << std::endl;
	std::cout << std::hex << Stack.size() << std::endl;
	for (int i = 0; i < Stack.size(); i++)
	{
		fout << Stack.at(i) << std::endl;
		std::cout << Stack.at(i) << std::endl;
	}
	fout << 0x0010 << std::endl;
	fout << 0xffff << std::endl;

	return 0;
}
