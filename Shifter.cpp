#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

//#include "CCData.h"

#include "TROOT.h"
#include "TFile.h"
#include "TBranch.h"
#include "TH1I.h"
#include "TTree.h"

struct Pattern		//Sort of rough compression indices, wil help moving faster
{
	int n;		//number of cluster in vector
	int s;		//size of cluster
};


void Mapper(unsigned int Slot, unsigned int Channel, std::string &ID, int &x, int &y, int &z);

int main(int argc, char** argv)
{
	gROOT->ProcessLine("#include <vector>");
	unsigned long long it = 12345;
	std::vector<unsigned long long> vDTB, vDAQ;	//vector from DAQ and DaTaBase timestamps

	std::vector<Pattern> vDAQ_C, vDTB_C;		//Vector for moving Cluster by cluster 

	unsigned int TOutN, AOutN, OutN, Trigger;
	float TDCres, ADCres = 0.488281;		//Resolution of CAMAC modules, ADC is 11bit

	unsigned long long TimeStamp, TimeSync, TimeBase;
	unsigned long long t0, t1;
	std::vector<unsigned int> PMTx, PMTy, PMTz;
	std::vector<float> Phys;
	std::vector<std::string> Detector;

	std::vector<std::string>  *Type;
	std::vector<unsigned int> *Value, *Slot, *Channel;

//	TBranch *b_Trigger, *b_OutN, *b_Type, *b_Value, *b_Slot, *b_Channel, *b_TimeStamp;

	int res;
	std::cout << "TDC resolution (0 = 0.5ns, 1 = 1.0ns, 2 = 2.0ns, 3 = 4.0ns) : ";
	std::cin >> res;

	switch (res)
	{
		case 0:
			TDCres = 0.5;
			break;
		case 1:
			TDCres = 1.0;
			break;
		case 2:
			TDCres = 2.0;
			break;
		case 3:
			TDCres = 4.0;
			break;
		default:
			TDCres = 4.0;
			break;
	}

	//Open raw tree
	TFile* in = new TFile(argv[1]);
	TTree* Otree = (TTree*) in->Get("CCData");

	Type = 0;
	Value = 0;
	Slot = 0;
	Channel = 0;

	Otree->SetBranchAddress("Trigger", &Trigger);
	Otree->SetBranchAddress("OutNumber", &OutN);
	Otree->SetBranchAddress("Type", &Type);
	Otree->SetBranchAddress("Value", &Value);
	Otree->SetBranchAddress("Slot", &Slot);
	Otree->SetBranchAddress("Channel", &Channel);
	Otree->SetBranchAddress("TimeStamp", &TimeStamp);


	unsigned int nent = Otree->GetEntries();

	//Set wget, obtain spill information from db
	std::string url = "wget -O webtime \"http://ifb-data.fnal.gov:8100/ifbeam/data/data?e=e%2C1d&b=BNBBPMTOR&f=csv&tz=&action=Show+device&t0=";
	std::stringstream cmd;
	Otree->GetEntry(0);
	t0 = TimeStamp;
	Otree->GetEntry(nent-1);
	while (t0 < TimeStamp)
	{
		cmd.clear();
		cmd.str("");
		cmd << url;
		cmd << std::fixed << std::setprecision(3) << (t0-1)/1000.0;
		t0 += 
		cmd << "&t1=" << (TimeStamp+1)/1000.0 << "\"";

	std::cout << cmd.str() << std::endl;
	system(cmd.str().c_str());
	
	//Output file
	std::string root_name = std::string(in->GetName());
	std::size_t pos = root_name.find(".root");
	root_name.erase(root_name.begin()+pos, root_name.end());
	root_name += "_post.root";					//Silly name for now
	TFile* out = new TFile(root_name.c_str(), "RECREATE");

	//Load tree vector and fill
	//
	//Set postprocessed tree
	TTree* Ntree = new TTree("CCData", "CCData");

	Ntree->Branch("Trigger", &Trigger, "Trigger/i");
	Ntree->Branch("OutNumber", &OutN, "OutN/i");
	Ntree->Branch("Type", &Type);			//string
	Ntree->Branch("Value", &Value);			//u int
	Ntree->Branch("Phys", &Phys);			//float
	Ntree->Branch("Slot", &Slot);			//u int
	Ntree->Branch("Channel", &Channel);		//u int
	Ntree->Branch("Detector", &Detector);		//string
	Ntree->Branch("PMTx", &PMTx);			//int
	Ntree->Branch("PMTy", &PMTy);			//int
	Ntree->Branch("PMTz", &PMTz);			//int
	Ntree->Branch("TimeStamp", &TimeStamp, "TimeStamp/l" );

	int x, y, z;
	std::string Device;
	for (unsigned int i = 0; i < nent; ++i)
	{
		Phys.clear();
		Detector.clear();
		PMTx.clear();
		PMTy.clear();
		PMTz.clear();

		Otree->GetEntry(i);
		vDAQ.push_back(TimeStamp);	//Get timestamp here, will fill later
		for (int j = 0; j < Value->size(); ++j)
		{
			if (Type->at(j) == "TDC")
				Phys.push_back(Value->at(j)*TDCres);
			if (Type->at(j) == "ADC")
				Phys.push_back(Value->at(j)*ADCres);

			Mapper(Slot->at(j), Channel->at(j), Device, x, y, z);
		
			Detector.push_back(Device);
			PMTx.push_back(x);
			PMTy.push_back(y);
			PMTz.push_back(z);
		}

		Ntree->Fill();			//Time to fill
	}

	//Load db vector
	std::fstream fin("webtime", std::fstream::in);
	std::string Line;
	while (getline(fin, Line))
	{
		pos = Line.find(",");
		Line.erase(Line.begin()+pos, Line.end());

		std::stringstream ssL;
		ssL << Line;
		ssL >> it;

		if (it == 0) continue;
		if (vDTB.size() == 0) vDTB.push_back(it);
		else if (it != vDTB.at(vDTB.size()-1))
			vDTB.push_back(it);
	}
	if (vDTB.size() == 0)
		std::cout << "Database is empty, no synchronisation available\n" << std::endl;

	//Algorithm starts here, synchronise daq times with dtb times

	long offset;

	Pattern WalkDAQ, WalkDTB;
	unsigned int iD = 0, iS = 0;

	while ((vDTB.size() != 0) && (iD+1 < vDAQ.size()))	//Starts only if dtb is not empty
	{
		vDAQ_C.clear();		//Clear clusters -> the loop is super cluster per super cluster
		vDTB_C.clear();

		while (iD+1 < vDAQ.size())	//Loop over DAQ data looking for clusters
		{
			WalkDAQ.s = 1;
			WalkDAQ.n = iD++;
			while (vDAQ.at(iD)-vDAQ.at(iD-1) < 100) 	//Size of cluster
			{
				++WalkDAQ.s;
				if (iD+1 < vDAQ.size())
					++iD;
				else break;
				
			}

			vDAQ_C.push_back(WalkDAQ);

//			Stops every super Cluster
			if (vDAQ.at(iD)-vDAQ.at(iD-1) > 2000) break;
		}

		while (iS+1 < vDTB.size())	//Same for DTB
		{
			WalkDTB.s = 1;
			WalkDTB.n = iS++;
			while (vDTB.at(iS)-vDTB.at(iS-1) < 100)
			{
				++WalkDTB.s;
				if (iS+1 < vDTB.size())
					++iS;
				else break;
			}

			vDTB_C.push_back(WalkDTB);

//			Stops every super Cluster
			if (vDTB.at(iS)-vDTB.at(iS-1) > 2000) break;
		}

//		Loop in reverse on DAQ clusters, trying to align them from the end
		for (int k = 0; k < vDAQ_C.size(); ++k)
		{
			if (iD+1 < vDAQ.size())
			{
				WalkDAQ = vDAQ_C.at(vDAQ_C.size()-1-k);
				if (k < vDTB_C.size())
				{
					WalkDTB = vDTB_C.at(vDTB_C.size()-1-k);
			
					offset = vDTB.at(WalkDTB.n) - vDAQ.at(WalkDAQ.n);
				}
			}
			else
			{
				WalkDAQ = vDAQ_C.at(k);
				if (k < vDTB_C.size())
				{
					WalkDTB = vDTB_C.at(k);
			
					offset = vDTB.at(WalkDTB.n) - vDAQ.at(WalkDAQ.n);
				}
			}

			for (int l = 0; l < WalkDAQ.s; ++l)
				vDAQ.at(WalkDAQ.n+l) += offset;
		}
	}
	//End of algorithm

	//Filling TimeSync Branch
	TBranch* SyncBranch = Ntree->Branch("TimeSync", &TimeSync, "TimeSync/l");
	for (unsigned int j = 0; j < nent; j++)
	{
		TimeSync = vDAQ.at(j);
		SyncBranch->Fill();
	}
	
	TTree *DataTree = new TTree("DB", "database");
	DataTree->Branch("TimeBase", &vDTB);
	DataTree->Fill();

	out->cd();
	Ntree->Write();
	DataTree->Write();

	//Cleaning
	fin.close();
	in->Close();

//	out->Write();
	out->Close();	

	return 0;
}


//TDC only
void Mapper(unsigned int Slot, unsigned int Channel, std::string &ID, int &x, int &y, int &z)
{
	if (Slot == 14)
	{
		ID = "VETO";
		switch (Channel)
		{
			case 1:
				x = 0;
				y = 0;
				z = 0;
				break;
			case 2:
				x = 0;
				y = 1;
				z = 0;
				break;
			case 3:
				x = 0;
				y = 2;
				z = 0;
				break;
			case 4:
				x = 0;
				y = 3;
				z = 0;
				break;
			case 5:
				x = 0;
				y = 4;
				z = 0;
				break;
			case 6:
				x = 0;
				y = 5;
				z = 0;
				break;
			case 7:
				x = 0;
				y = 6;
				z = 0;
				break;
			case 8:
				x = 0;
				y = 7;
				z = 0;
				break;
			case 9:
				x = 0;
				y = 8;
				z = 0;
				break;
			case 10:
				x = 0;
				y = 9;
				z = 0;
				break;
			case 11:
				x = 0;
				y = 10;
				z = 0;
				break;
			case 12:
				x = 0;
				y = 11;
				z = 0;
				break;
			case 13:
				x = 1;
				y = 12;
				z = 0;
				break;
			case 14:
				x = 1;
				y = 0;
				z = 0;
				break;
			case 15:
				x = 1;
				y = 1;
				z = 0;
				break;
			case 16:
				x = 1;
				y = 2;
				z = 0;
				break;
			case 17:
				x = 1;
				y = 3;
				z = 0;
				break;
			case 18:
				x = 1;
				y = 4;
				z = 0;
				break;
			case 19:
				x = 1;
				y = 5;
				z = 0;
				break;
			case 20:
				x = 1;
				y = 6;
				z = 0;
				break;
			case 21:
				x = 1;
				y = 7;
				z = 0;
				break;
			case 22:
				x = 1;
				y = 8;
				z = 0;
				break;
			case 23:
				x = 1;
				y = 9;
				z = 0;
				break;
			case 24:
				x = 1;
				y = 10;
				z = 0;
				break;
			case 25:
				x = 1;
				y = 11;
				z = 0;
				break;
			case 26:
				x = 1;
				y = 12;
				z = 0;
				break;

			default:
				x = -10;
				y = -10;
				z = -10;
				break;
		}
	}
	else if (Slot == 17)
	{
		ID = "MRD";
		switch (Channel)
		{
			case 1:
				x = 0;
				y = 0;
				z = 2;
				break;
			case 2:
				x = 0;
				y = 1;
				z = 2;
				break;
			case 3:
				x = 0;
				y = 2;
				z = 2;
				break;
			case 4:
				x = 0;
				y = 3;
				z = 2;
				break;
			case 5:
				x = 0;
				y = 4;
				z = 2;
				break;
			case 6:
				x = 0;
				y = 5;
				z = 2;
				break;
			case 7:
				x = 0;
				y = 6;
				z = 2;
				break;
			case 8:
				x = 0;
				y = 7;
				z = 2;
				break;
			case 9:
				x = 0;
				y = 8;
				z = 2;
				break;
			case 10:
				x = 0;
				y = 9;
				z = 2;
				break;
			case 11:
				x = 0;
				y = 10;
				z = 2;
				break;
			case 12:
				x = 0;
				y = 11;
				z = 2;
				break;
			case 13:
				x = 0;
				y = 12;
				z = 2;
				break;
			case 14:
				x = 1;
				y = 0;
				z = 3;
				break;
			case 15:
				x = 1;
				y = 1;
				z = 3;
				break;
			case 16:
				x = 1;
				y = 2;
				z = 3;
				break;
			case 17:
				x = 1;
				y = 3;
				z = 3;
				break;
			case 18:
				x = 1;
				y = 4;
				z = 3;
				break;
			case 19:
				x = 1;
				y = 5;
				z = 3;
				break;
			case 20:
				x = 1;
				y = 6;
				z = 3;
				break;
			case 21:
				x = 1;
				y = 7;
				z = 3;
				break;
			case 22:
				x = 1;
				y = 8;
				z = 3;
				break;
			case 23:
				x = 1;
				y = 9;
				z = 3;
				break;
			case 24:
				x = 1;
				y = 10;
				z = 3;
				break;
			case 25:
				x = 1;
				y = 11;
				z = 3;
				break;
			case 26:
				x = 1;
				y = 12;
				z = 3;
				break;

			default:
				x = -10;
				y = -10;
				z = -10;
				break;
		}
	}
	else if (Slot == 18)
	{
		ID = "MRD";
		switch (Channel)
		{
			case 1:
				x = 0;
				y = 0;
				z = 3;
				break;
			case 2:
				x = 1;
				y = 0;
				z = 3;
				break;
			case 3:
				x = 2;
				y = 0;
				z = 3;
				break;
			case 4:
				x = 3;
				y = 0;
				z = 3;
				break;
			case 5:
				x = 4;
				y = 0;
				z = 3;
				break;
			case 6:
				x = 5;
				y = 0;
				z = 3;
				break;
			case 7:
				x = 6;
				y = 0;
				z = 3;
				break;
			case 8:
				x = 7;
				y = 0;
				z = 3;
				break;
			case 9:
				x = 8;
				y = 0;
				z = 3;
				break;
			case 10:
				x = 9;
				y = 0;
				z = 3;
				break;
			case 11:
				x = 10;
				y = 0;
				z = 3;
				break;
			case 12:
				x = 11;
				y = 0;
				z = 3;
				break;
			case 13:
				x = 12;
				y = 0;
				z = 3;
				break;
			case 14:
				x = 0;
				y = 1;
				z = 3;
				break;
			case 15:
				x = 1;
				y = 1;
				z = 3;
				break;
			case 16:
				x = 2;
				y = 1;
				z = 3;
				break;
			case 17:
				x = 3;
				y = 1;
				z = 3;
				break;
			case 18:
				x = 4;
				y = 1;
				z = 3;
				break;
			case 19:
				x = 5;
				y = 1;
				z = 3;
				break;
			case 20:
				x = 6;
				y = 1;
				z = 3;
				break;
			case 21:
				x = 7;
				y = 1;
				z = 3;
				break;
			case 22:
				x = 8;
				y = 1;
				z = 3;
				break;
			case 23:
				x = 9;
				y = 1;
				z = 3;
				break;
			case 24:
				x = 10;
				y = 1;
				z = 3;
				break;
			case 25:
				x = 11;
				y = 1;
				z = 3;
				break;
			case 26:
				x = 12;
				y = 1;
				z = 3;
				break;

			default:
				x = -10;
				y = -10;
				z = -10;
				break;
		}
	}
}
