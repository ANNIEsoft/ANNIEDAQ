#ifndef UCADC_H_E23306AB
#define UCADC_H_E23306AB

#include <time.h>
#include <vme/vme.h>
#include <vme/vme_api.h>

class VMEBus
{
	public:
		vme_bus_handle_t* handle( void );
		VMEBus( void );
		VMEBus(vme_bus_handle_t handle);
		~VMEBus( void );
	private:
		bool external_handle;
		vme_bus_handle_t bus_handle;
};

class MasterWindow
{
	public:
		
		vme_master_handle_t* handle(void);
		uint32_t *map(void);
		void unMap(void);
		int GetSlot( void ) {
			return current_slot;
		}
		void switchSlot(int slot);
		MasterWindow (vme_bus_handle_t* in_bus_handle, uint32_t offset = 0);
		~MasterWindow(void);
	private:
		static const int AM = VME_A32UD;
		static const unsigned int databytes = 0x48;
		vme_bus_handle_t* bus_handle;
		vme_master_handle_t window_handle;
		uint8_t current_slot;
		uint32_t base_address;
		uint32_t window_offset;

};

class CardControl
{
	public:
		int checkCard(int);
		void setCard(int);
		void setValue(uint8_t address, uint32_t value);
		void setValue(unsigned int slot, uint8_t address, uint32_t value);
		uint32_t readValue(uint8_t address);
		uint32_t readValue(unsigned int slot, uint8_t address);
		void TestModeOn(unsigned int slot);
		void TestModeOff(unsigned int slot);
		void SetSamples(unsigned int slot, unsigned int perTrigger, unsigned int total);
		void SetUntriggeredMode(unsigned int slot);
		void SetTriggerMode(unsigned int slot);
		void ResetTriggerCounters(unsigned int slot);
		int GetMemoryFull(unsigned int slot);
		CardControl (vme_bus_handle_t* in_bus_handle);
		~CardControl ( void );
	private:
		MasterWindow control_window;
		vme_master_handle_t* window_handle;
		volatile uint32_t *control_ptr;
		int current_card;
};

class CardTime
{
	public:
		uint64_t getValue(int card);
		int avgTime(struct timespec *avg);
		CardTime (vme_bus_handle_t* in_bus_handle);
		~CardTime ( void );
	private:
		int current_card;
		MasterWindow time_window;
		vme_master_handle_t* window_handle;
		volatile uint32_t *cardtime_ptr;
		struct timespec before, after;
};

class DMABuffer
{
	public:
		uint32_t* getPointer(void);
		int readCard(int slot, int bank);
		int readTimeBuffers(int slot);
		static unsigned int databytes;
		static int getDataBytes();
		static void setDataBytes(int bytes);
                DMABuffer(vme_bus_handle_t* in_bus_handle);
		~DMABuffer(void);
	private:
		static const int AM = VME_A32UB;
		const static unsigned int bank_address[];
		vme_bus_handle_t *bus_handle;
		vme_dma_handle_t dma_handle;
		uint32_t *data_ptr;
};

class EventMemory 
{
	public:
		int storeBank(int card, int bank, int length, void* data);
		EventMemory(int incards);
		~EventMemory( void );
	private:
		int cards;
		char ***Array;
};


#endif /* end of include guard: UCADC_H_E23306AB */

