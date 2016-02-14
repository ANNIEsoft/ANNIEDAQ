#include <exception>
#include <stdexcept>
#include <string.h>
#include <time.h>

#include "ucadc.h"

vme_bus_handle_t* VMEBus::handle( void) {
	return &bus_handle;
}

VMEBus::VMEBus( void ) {
	external_handle = false;
	if (vme_init(&bus_handle)) throw std::runtime_error("Error initializing the VMEBus");
}

VMEBus::VMEBus(vme_bus_handle_t handle) {
	external_handle = true;
	bus_handle = handle;
}

VMEBus::~VMEBus( void ) {
	if (external_handle) return;
	vme_term(bus_handle);
}

vme_master_handle_t* MasterWindow::handle(void){
	return &window_handle;
}
uint32_t* MasterWindow::map(void) {
	return (uint32_t*)vme_master_window_map(*bus_handle, window_handle, 0);
}
void MasterWindow::unMap(void) {	
	vme_master_window_unmap(*bus_handle, window_handle);
}
void MasterWindow::switchSlot(int slot) {
	if (current_slot==slot) return;
	current_slot = slot;
	base_address = current_slot * (uint32_t)0x8000000 + window_offset;
	if (vme_master_window_translate(*bus_handle, window_handle, base_address)) {
		throw std::runtime_error("Error translating the master window");
	}
	return;
}
MasterWindow::MasterWindow (vme_bus_handle_t* in_bus_handle, uint32_t offset){
	bus_handle = in_bus_handle;
	current_slot = 2;
	window_offset = offset;
	base_address = current_slot * (uint32_t)0x8000000 + offset;
	if (vme_master_window_create(*bus_handle, &window_handle,
			base_address, AM, databytes,
			VME_CTL_PWEN|VME_CTL_EXCLUSIVE, NULL)) {	// turn on write posting for the master window and exclusive control to enable translates.
		throw std::runtime_error("Error creating the master window");
	}
}
MasterWindow::~MasterWindow(void) {
	vme_master_window_release(*bus_handle, window_handle);
}

int CardControl::checkCard(int slot){
	control_window.switchSlot(slot);
	return control_ptr[0]>>8 == 0xdeadbe;  // return 1 if the card exists
}
void CardControl::setCard(int slot){
	control_window.switchSlot(slot);
	current_card = slot;
}
void CardControl::setValue(unsigned int slot, uint8_t address, uint32_t value) {
	if ((int)slot != current_card) {
		setCard(slot);
	}
	control_ptr[address] = value;
}
void CardControl::setValue(uint8_t address, uint32_t value) {
	control_ptr[address] = value;
}

uint32_t CardControl::readValue(unsigned int slot, uint8_t address) {
	if ((int)slot != current_card) {
		setCard(slot);
	}
	return control_ptr[address];
}
uint32_t CardControl::readValue(uint8_t address) {
	return control_ptr[address];
}

void CardControl::TestModeOn(unsigned int slot) {
	setValue(slot,0xF,1);
}
void CardControl::TestModeOff(unsigned int slot) {
	setValue(slot,0xF,0);
}
void CardControl::SetSamples(unsigned int slot, unsigned int perTrigger, unsigned int total) {
	setValue(slot,0x1,perTrigger-1);  // The current firmware actually goes one sample too far.
	setValue(slot,0x2,total);
}
void CardControl::SetUntriggeredMode(unsigned int slot) {
	setValue(slot,0xD,0x0);
}
void CardControl::SetTriggerMode(unsigned int slot) {
	setValue(slot,0xD,0x1);
}
void CardControl::ResetTriggerCounters(unsigned int slot) {
	setValue(slot,0x11,0x1);
}
int CardControl::GetMemoryFull(unsigned int slot) {
	return readValue(slot,0xA)&0x1;
}


CardControl::CardControl (vme_bus_handle_t* in_bus_handle) :
control_window(in_bus_handle)
{
	control_ptr = control_window.map();
	if (!control_ptr) throw std::runtime_error("Error mapping the window");
	current_card=-1;
}
CardControl::~CardControl ( void ) {
	control_window.unMap();
}

CardTime::CardTime (vme_bus_handle_t* in_bus_handle) :
time_window(in_bus_handle,0x4001000)
{
	cardtime_ptr = time_window.map();
	if (!cardtime_ptr) throw std::runtime_error("Error mapping the window");
	current_card=-1;
}
CardTime::~CardTime( void) {
	time_window.unMap();
}
uint64_t CardTime::getValue(int card) {
	uint64_t counter = 0;
	if (card != current_card) {
		time_window.switchSlot(card);
		current_card = card;
	}
	clock_gettime(CLOCK_REALTIME, &before);
	counter += cardtime_ptr[0];
	clock_gettime(CLOCK_REALTIME, &after);
	counter += ((uint64_t)cardtime_ptr[1])<<32;
	return counter;
}
int CardTime::avgTime(struct timespec *avg) {
	avg->tv_sec = (after.tv_sec - before.tv_sec)/2 + before.tv_sec;
	avg->tv_nsec = (before.tv_nsec+after.tv_nsec)/2;
	if ((after.tv_sec - before.tv_sec)%2 == 1) avg->tv_nsec += 500000000;
	return 1;
}
uint32_t* DMABuffer::getPointer(void) {
	return data_ptr;
}

const unsigned int DMABuffer::bank_address[] = {0x900000, 0x700000, 0x100000, 0xa00000, 0xb00000, 0xc00000, 0xd00000, 0xe00000};


int DMABuffer::readCard(int slot, int bank) {
	return vme_dma_read(*bus_handle, dma_handle, 0, (slot*0x8000000)+DMABuffer::bank_address[bank], AM, databytes, 0);
}
int DMABuffer::readTimeBuffers(int slot) {
	return vme_dma_read(*bus_handle, dma_handle, 0, (slot*0x8000000)+0x4000000, AM, 0x1000, 0);
}

unsigned int DMABuffer::databytes = 40000;

int DMABuffer::getDataBytes() {
	return DMABuffer::databytes;
}
void DMABuffer::setDataBytes(int bytes) {
	DMABuffer::databytes = bytes;
}

DMABuffer::DMABuffer(vme_bus_handle_t* in_bus_handle) {
	bus_handle = in_bus_handle;
	if (vme_dma_buffer_create(*bus_handle, &dma_handle, databytes, 0, NULL)) {
		throw std::runtime_error("Error creating DMA buffer.");
	}
	data_ptr = (uint32_t*)vme_dma_buffer_map(*bus_handle, dma_handle, 0);
	if (!data_ptr) {
		throw std::runtime_error("Error mapping DMA buffer.");
	}
}
DMABuffer::~DMABuffer(void) {
	vme_dma_buffer_unmap(*bus_handle, dma_handle);
	vme_dma_buffer_release(*bus_handle, dma_handle);
}

int EventMemory::storeBank(int card, int bank, int length, void* data){
	memcpy(Array[card][bank], data, length);
	return 0;
}
EventMemory::EventMemory(int incards) {
	cards = incards;
	Array = new char**[cards];
	for (int i = 0; i < cards; ++i) {
		Array[i] = new char*[8]; // 8 banks/card
		for (int j = 0; j < 8; ++j) {
			Array[i][j] = new char[DMABuffer::databytes];
		}
	}
}
EventMemory::~EventMemory( void ) {
	for (int i = 0; i < cards; ++i) {
		for (int j = 0; j < 8; ++j) {
			delete [] Array[i][j];
		}
		delete [] Array[i];
	}
	delete [] Array;
}


