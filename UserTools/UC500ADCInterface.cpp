//
//  UC500ADCInterface.cc
//  
//
//  Created by Jonathan Eisch on 2/4/16.
//
//

#include "UC500ADCInterface.h"
#include "CardData.h"

#include <time.h>
#include <vector>
#include <cstring>
#include <stdint.h>
#include <iostream>

#include "ucadc.h"


#define MAXSLOTS 22

bool UC500ADCInterface::Triggered() {
	for (std::vector<int>::iterator it = cards.begin(); it != cards.end(); it++) {
		if (control->GetMemoryFull(*it)) return 1;
	}
	return 0;
}

bool UC500ADCInterface::Triggered(int card) {
	if (control->GetMemoryFull(card)) return 1;
	return 0;
}

int UC500ADCInterface::NumberOfCards() {
	return cards.size();
}

CardData* UC500ADCInterface::GetCardData(int card) {
	int geographicalCard = cards[card];
	CardData *data= new CardData;
	control->TestModeOn(geographicalCard);
	data->LastSync = -1;
	data->SequenceID = -1;
	data->StartTime = start.tv_sec;
	data->CardID = geographicalCard;
	data->channels = 4;
	data->PMTID = new int[4];
	//data.PMTID[] = {1,2,3,4};
	data->buffersize = 4*10000;
	data->fullbuffsize = data->channels * data->buffersize;
	data->Data = new uint16_t[data->fullbuffsize];
	for (int bank = 0; bank < 8; bank++) {
		dma->readCard(geographicalCard,bank);
		memcpy(data->Data+bank*dma->databytes/2, data_ptr, dma->databytes);
	}
	control->ResetTriggerCounters(geographicalCard);
	return data;
}

bool UC500ADCInterface::Initialise() {
	for (std::vector<int>::iterator it = cards.begin(); it != cards.end(); it++) {
		control->TestModeOn(*it);
		control->SetSamples(*it,10000,10000);
		control->SetTriggerMode(*it);
		control->ResetTriggerCounters(*it);
	}
	return 1;
}

bool UC500ADCInterface::Finalise() {
	for (std::vector<int>::iterator it = cards.begin(); it != cards.end(); it++) {
		control->TestModeOn(*it);
		control->ResetTriggerCounters(*it);
		control->SetUntriggeredMode(*it);
	}
	return 1;
}

bool UC500ADCInterface::EnableTrigger() {
	for (std::vector<int>::iterator it = cards.begin(); it != cards.end(); it++) {
		control->TestModeOff(*it);
	}
	return 1;
}

UC500ADCInterface::UC500ADCInterface (int crate) {
	bus = new VMEBus();
	control = new CardControl(bus->handle());
	cardtime = new CardTime(bus->handle());
	dma = new DMABuffer(bus->handle());
	data_ptr = dma->getPointer();
	
	UC500ADCInterface::crate = crate;
	clock_gettime(CLOCK_REALTIME, &start);
	
	for (int i=1;i<=MAXSLOTS;i++) {
		if (control->checkCard(i)) cards.push_back(i);
	}
}
UC500ADCInterface::~UC500ADCInterface( void ) {
	delete dma;
	delete cardtime;
	delete control;
	delete bus;
}
