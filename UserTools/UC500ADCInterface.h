#ifndef UC500ADCINTERFACE_H
#define UC500ADCINTERFACE_H

#include <list>
#include <time.h>

#include "HardwareInterface.h"
#include "CardData.h"
#include "ucadc.h"


class UC500ADCInterface: virtual public HardwareInterface {
public:
	bool Triggered();
	bool Triggered(int card);
	int NumberOfCards();
	CardData* GetCardData(int card);
	bool Initialise();
	bool Finalise();
	bool EnableTrigger();
	UC500ADCInterface(int crate);
	~UC500ADCInterface( void );
	
private:

	int crate;
	struct timespec start;
	std::list<int> cards;
	VMEBus *bus;
	CardControl *control;
	CardTime *cardtime;
	DMABuffer *dma;
	EventMemory *event;
	uint32_t *data_ptr;
	
};

#endif
