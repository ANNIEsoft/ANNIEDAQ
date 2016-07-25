#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#include "TFile.h"
#include "TH1I.h"
#include "TTree.h"

void Mapper(unsigned int Slot, unsigned int Channel, int &ID, int &x, int &y, int &z);

int main(int argc, char** argv)
{
	unsigned long long it = 12345;
	std::vector<unsigned long long> vSpill, vDAQ;

	unsigned int TOutN, AOutN, OutN, Trigger;
	unsigned int TSlot[512], TChannel[512];
	unsigned int ASlot[512], AChannel[512];
	int TDCi[512], ADCi[512];
	float TDCf[512], ADCf[512];
	int Detector[512], PMTx[512], PMTy[512], PMTz[512];			//PMTxyz give the PMT location
	unsigned long long TimeStamp, TimeStampSync;	//Detector is 0 for VETO, 1 for MRD
	float TDCres = 4.0, ADCres = 1.0;		//Resolution of CAMAC modules, hardcoded for now
										

	//Open raw tree
	TFile* in = new TFile(argv[1]);
	TTree* Otree = (TTree*) in->Get("CCData");

	Otree->SetBranchStatus("*", 1);
	Otree->SetBranchAddress("Trigger", &Trigger);
	Otree->SetBranchAddress("OutNumber", &OutN);
	Otree->SetBranchAddress("TDC", &TDCi);
	Otree->SetBranchAddress("TSlot", &TSlot);
	Otree->SetBranchAddress("TChannel", &TChannel);
	Otree->SetBranchAddress("ADC", &ADCi);
	Otree->SetBranchAddress("ASlot", &ASlot);
	Otree->SetBranchAddress("AChannel", &AChannel);
	Otree->SetBranchAddress("TimeStamp", &TimeStamp);

	unsigned int nent = Otree->GetEntries();

	//Set wget, obtain spill information from db
	std::string url = "wget -O webtime \"http://ifb-data.fnal.gov:8100/ifbeam/data/data?e=e%2C1d&b=BNBBPMTOR&f=csv&tz=&action=Show+device&t0=";
	std::stringstream cmd;
	cmd << url;
	Otree->GetEntry(0);
	cmd << std::fixed << std::setprecision(3) << TimeStamp/1000.0;
	Otree->GetEntry(nent-1);
	cmd << "&t1=" << TimeStamp/1000.0 << "\"";

	std::cout << cmd.str() << std::endl;
	system(cmd.str().c_str());
	
	//Output file
	std::string root_name = std::string(in->GetName());
	std::size_t pos = root_name.find(".root");
	root_name.erase(root_name.begin()+pos, root_name.end());
	root_name += "post.root";					//Silly name for now
	TFile* out = new TFile(root_name.c_str(), "RECREATE");

	//Load tree vector and fill
	//
	//Set postprocessed tree
	TTree* Ntree = new TTree("CCData", "CCData");

	Ntree->Branch("Trigger", &Trigger, "Trigger/i");
	Ntree->Branch("OutNumber", &OutN, "OutN/i");
	Ntree->Branch("TDCi", TDCi, "TDCi[OutN]/i");
	Ntree->Branch("TDCf", TDCf, "TDCf[OutN]/F");
	Ntree->Branch("TSlot", &TSlot, "TSlot[OutN]/i");
	Ntree->Branch("TChannel", &TChannel, "TChannel[OutN]/i" );
	Ntree->Branch("ADCi", ADCi, "ADCi[OutN]/i");
	Ntree->Branch("ADCf", ADCf, "ADCf[OutN]/F");
	Ntree->Branch("ASlot", &ASlot, "ASlot[OutN]/i");
	Ntree->Branch("AChannel", &AChannel, "AChannel[OutN]/i" );
	Ntree->Branch("TimeStamp", &TimeStamp, "TimeStamp/l" );
	Ntree->Branch("Detector", Detector, "Detector[OutN]/I" );
	Ntree->Branch("PMTx", PMTx, "PMTx[OutN]/I" );
	Ntree->Branch("PMTy", PMTy, "PMTy[OutN]/I" );
	Ntree->Branch("PMTz", PMTz, "PMTz[OutN]/I" );

	for (unsigned int i = 0; i < nent; ++i)
	{
		Otree->GetEntry(i);
		vDAQ.push_back(TimeStamp);		//Get timestamp here, will fill later
		for (int j = 0; j < OutN; j++)		//Create float arrays
		{
			TDCf[j] = TDCi[j]*TDCres;
			ADCf[j] = ADCi[j]*ADCres;

			Mapper(TSlot[j], TChannel[j], Detector[j], PMTx[j], PMTy[j], PMTz[j]);	//Mapping complete
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
		if (vSpill.size() == 0) vSpill.push_back(it);
		else if (it != vSpill.at(vSpill.size()-1))
			vSpill.push_back(it);
	}
	if (vSpill.size() == 0)
		std::cout << "Database is empty, no synchronisation abailable\n" << std::endl;

	//Algorithm starts here, synchronise daq times with db times
	int i = 0, k = 0, jA = 0, jB = 0;
	unsigned long offset;
	while (jB < vDAQ.size())
	{
		while (++jB < vDAQ.size())
			if (fabs(vDAQ.at(jB)-vDAQ.at(jB-1)) > 100)	//Not part of the pulse train
				break;

		while (i < vSpill.size())
		{
			if (vSpill.at(i) > vDAQ.at(jA))
				break;
			++i;
		}

		if (vSpill.size() == 0) offset = 0;
		else offset = vSpill.at(i) - vDAQ.at(jA);	//Compute the offset

		//Move by offset
		for (int k = jA; k < jB; ++k)
			vDAQ.at(k) += offset;

		//Move indices
		jA = jB;
	}
	//End of algorithm

	//Filling TimeSync Branch
	TBranch* TimeSync = Ntree->Branch("TimeStampSync", &TimeStampSync, "TimeStampSync/l" );
	for (int j = 0; j < nent; j++)
	{
		TimeStampSync = vDAQ.at(j);
		TimeSync->Fill();
	}



	/*
	int nbins = vDAQ.at(vDAQ.size()-1)-vDAQ.at(0)+20000;
	TH1I* daq = new TH1I("daq", "DAQ", nbins, vDAQ.at(0)-10000, vDAQ.at(vDAQ.size()-1)+10000);
	TH1I* spill = new TH1I("spill", "SPILL", nbins, vDAQ.at(0)-10000, vDAQ.at(vDAQ.size()-1)+10000);

	for (i = 0; i < vSpill.size(); ++i)
		spill->Fill(vSpill.at(i));
	for (i = 0; i < vDAQ.size(); ++i)
		daq->Fill(vDAQ.at(i));
	*/

	out->cd();
	Ntree->Write();

	//Cleaning
	fin.close();
	in->Close();

//	out->Write();
	out->Close();	

	return 0;
}

void Mapper(unsigned int Slot, unsigned int Channel, int &ID, int &x, int &y, int &z)
{
	if (Slot == 14)
	{
		ID = 0;
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
		ID = 1;
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
		ID = 1;
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
