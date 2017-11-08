#include "timestamp.h"
#include <stdint.h>
#include <bitset>
#include <iostream>
#include <iomanip>

bool TriggerTimeStamp::Set(uint32_t new_triggermask, uint32_t new_counter) {
	triggermask = new_triggermask;
	counter = new_counter;
	return true;
}

TriggerTimeStamp::TriggerTimeStamp(uint32_t new_triggermask, uint32_t new_counter) {
	triggermask = new_triggermask;
	counter = new_counter;
}

TriggerTimeStamp::TriggerTimeStamp(const TriggerTimeStamp &obj) {
	triggermask = obj.TriggerMask();
	counter = obj.Counter();
}

uint32_t TriggerTimeStamp::TriggerMask() const {
	return triggermask;
}

uint32_t TriggerTimeStamp::Counter() const {
	return counter;
}

std::ostream& operator<<(std::ostream& os, const TriggerTimeStamp& obj) {
	std::bitset<32> t(obj.TriggerMask());
	os << t << " 0x" << std::setfill('0') << std::setw(8) << std::hex << obj.Counter();
	return os;
}

