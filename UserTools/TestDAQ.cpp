#include "TestDAQ.h"

TestDAQ::TestDAQ():Tool(){}


bool TestDAQ::Initialise(std::string configfile, DataModel &data)
{

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();
	
	m_data= &data;
	Test = new zmq::socket_t(*(m_data->context), ZMQ_REQ);
	std::cout << "d0" << std::endl;
//	Test->setsockopt(ZMQ_RCVTIMEO, 1000);
	std::cout << "d1" << std::endl;
	Test->bind("ipc:///tmp/0");
	std::cout << "d2" << std::endl;
	
	return true;
}


bool TestDAQ::Execute()
{
	zmq::message_t msg(256);
	std::stringstream tmp;
	tmp << "Gimme";

	snprintf ((char *) msg.data(), 512, "%s", tmp.str().c_str());
	if (Test->send(msg))
	{
		zmq::message_t txt;
		if (Test->recv(&txt))
		{
			std::istringstream iss(static_cast<char*>(txt.data()));
			std::string arg1;
			unsigned long arg2;		
		
			iss >> arg1 >> std::hex >> arg2;
		
			if (arg1 == "TTree")
				std::cout << "TestDAQ : " << std::hex << arg2 << std::endl;
		}
		else std::cout << "Time out" << std::endl;
	}

	std::cout << "Exe ended" << std::endl;

	return true;
}


bool TestDAQ::Finalise()
{

	delete Test;
	Test = 0;

	return true;
}
