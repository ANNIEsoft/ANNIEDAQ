#include "TreeRecorder.h"

TreeRecorder::TreeRecorder():Tool(){}

bool TreeRecorder::Initialise(std::string configfile, DataModel &data){

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;

	m_variables.Get("TreeCap", TreeCap); 
	m_variables.Get("FileCap", FileCap);
	m_variables.Get("OutPath", OutPath);
	m_variables.Get("OutName", OutName);
	m_variables.Get("configcc", configcc);		//Module slots

	FileCount = 0;	
	std::stringstream Tmp;
	OutName = OutPath + OutName;
	Tmp << OutName << "p" << FileCount << ".root";
//	TFile file(Tmp.str().c_str(), "RECREATE", "", 1);
//	file.Write();
//	file.Close();
	
	Isend = new zmq::socket_t(*(m_data->context), ZMQ_PUSH);
	Isend->bind("inproc://RootWriter");
	
	args = new card_root_thread_args(OutName, m_data->context, FileCap, FileCount);
	
	h1 = new TThread("h1", TreeRecorder::RootWriter, args);
	h1->Run();

	std::string Line;
	std::stringstream ssL;
	std::string sEmp;
	int iEmp;

	tree = new TTree("CCData", "ccdata");

//Edit starts here, Wed 1 Jun, ...
	tree->Branch("Trigger", &Trigger, "Trigger/i");
	tree->Branch("OutNumber", &OutN, "OutN/i");
	tree->Branch("TDC", TDC, "TDC[OutN]/i");
	tree->Branch("TCardID", &TCardID, "TCardID[OutN]/i");
	tree->Branch("TChannel", &TChannel, "TChannel[OutN]/i" );
	tree->Branch("ADC", ADC, "ADC[OutN]/i");
	tree->Branch("ACardID", &ACardID, "ACardID[OutN]/i");
	tree->Branch("AChannel", &AChannel, "AChannel[OutN]/i" );
//... and ends here

/*
	std::ifstream fin (configcc.c_str());
	while (getline(fin, Line))
	{
		if (Line[0] == '#') continue;
		else
		{
			ssL.str("");
			ssL.clear();
			ssL << Line;
			if (ssL.str() != "")
			{
				ssL >> sEmp;
				Lcard.push_back(sEmp);		//Modeli L
				ssL >> iEmp;
				Ncard.push_back(iEmp);		//Slot N
			}
		}
	}
	fin.close();

	for (int i = 0; i < Lcard.size(); i++)
	{
		if (Lcard.at(i) == "Lecroy3377")
		{
			ssL.str("");
			ssL.clear();
			ssL << "TDC" << Ncard.at(i);
			sEmp = ssL.str();
			ssL << "[32]/I";
			vTDC.push_back(new int[32]);
			tree->Branch(sEmp.c_str(), vTDC.at(vTDC.size()-1), ssL.str().c_str());
		}
		else if (Lcard.at(i) == "Lecroy4300b")
		{
			ssL.str("");
			ssL.clear();
			ssL << "ADC" << Ncard.at(i);
			sEmp = ssL.str();
			ssL << "[16]/I";
			vADC.push_back(new int[16]);
			tree->Branch(sEmp.c_str(), vTDC.at(vTDC.size()-1), ssL.str().c_str());
		}
		else if (Lcard.at(i) != "CamacCrate")
			std::cout << "\n\nUnkown card\n" << std::endl;
	}
*/
	Trigger = 0;
	ThreadCount = 0;
	return true;
}

bool TreeRecorder::Execute()
{
	if (m_data->TRG)
	{
		Trigger++;
		TOutN = 0, AOutN = 0;

		int i, j;
		if (m_data->List.Data.size() > 0)
		{
			i = 0, j = 0;
			is = m_data->List.Data["TDC"].Num.begin();
			for (; is != m_data->List.Data["TDC"].Num.end(); is++, i++)
			{
				it = is->ch.begin();
				TOutN += is->ch.size();	//number of channels on
				for (; it != is->ch.end(); it++, j++)
				{
					*(TCardID+j) = m_data->List.Data["TDC"].Slot.at(i);
					*(TChannel+j) = it->first;
					*(TDC+j) = it->second;
				}
			}

			i = 0, j = 0;
			is = m_data->List.Data["ADC"].Num.begin();
			for (; is != m_data->List.Data["ADC"].Num.end(); is++, i++)
			{
				it = is->ch.begin();
				AOutN += is->ch.size();	//number of channels on
				for (; it != is->ch.end(); it++, j++)
				{
					*(ACardID+j) = m_data->List.Data["ADC"].Slot.at(i);
					*(AChannel+j) = it->first;
					*(ADC+j) = it->second;
				}
			}
		}

		m_data->List.Data.clear();		

		OutN = (TOutN >= AOutN) ? TOutN : AOutN;
	
		tree->Fill();
	
		if (tree->GetEntriesFast() >= TreeCap*(1+ThreadCount))
		{
			zmq::message_t message(512);
			std::stringstream tmptree;
			tmptree << "TTree " << tree;
		
			snprintf ((char *) message.data(), 512, "%s", tmptree.str().c_str()) ;
			Isend->send(message);
			if (++ThreadCount >= FileCap) 
				ThreadCount = 0;
		}
	}
	
	return true;
}
	
bool TreeRecorder::Finalise(){

	zmq::message_t message(256);
	std::string send = "Quit 0x00";
	snprintf ((char *) message.data(), 256 , "%s" ,send.c_str()) ;
	Isend->send(message);

	h1->Join();

	std::stringstream tmp;
	tmp << OutName << "p" << FileCount << ".root";
	TFile file(tmp.str().c_str(), "UPDATE", "", 1);
	
	tree->Write();
	file.Write();
	file.Close();

//	for (int i = 0; i < vTDC.size(); i++)
//		delete vTDC.at(i);
//	for (int i = 0; i < vADC.size(); i++)
//		delete vADC.at(i);


	delete tree;
	tree = 0;

	delete h1;
	h1 = 0;

	delete Isend;
	Isend = 0;
	
	delete args;
	args = 0;

	for (int i = 0; i < m_data->List.CC.size(); i++)
	{
		delete m_data->List.CC["TDC"].at(i);
		delete m_data->List.CC["ADC"].at(i);
	}

	m_data->List.CC.clear();
	m_data->List.Data.clear();
	
	return true;
}


void* TreeRecorder::RootWriter(void* arg)
{	
	card_root_thread_args* args = static_cast<card_root_thread_args*>(arg);
 
	zmq::socket_t Ireceive (*(args->context), ZMQ_PULL);
	Ireceive.connect("inproc://RootWriter");

	bool running = true;
	*(args->part) = 0;
	int Count = 0;	 
	std::string Opt = "RECREATE";
	
	while (running)
	{
		zmq::message_t comm;
		Ireceive.recv(&comm);
		
		std::istringstream iss(static_cast<char*>(comm.data()));
		std::string arg1 = "";
		long long unsigned int arg2;		
		TTree *tree;
		
		iss >> arg1 >> std::hex >> arg2;

		if (arg1 == "TTree")
		{
			std::stringstream tmp;
			tmp << args->out << "p" << *(args->part) << ".root";
			TFile file(tmp.str().c_str(), Opt.c_str(), "", 1);

			tree = reinterpret_cast<TTree*>(arg2);
			tree->Write();

			file.Write();
			file.Close();

			Opt = "UPDATE";

			if (++Count >= args->cap)
			{
				(*(args->part))++;	//FileCount
				Count = 0;	
				Opt = "RECREATE";
				tree->Reset();
				tree = 0;
			}
		}
		else if (arg1 == "Quit")
			running = false;
	}
}
