#include "eventlog.h"
#include <stdint.h>
#include <bitset>
#include <iostream>
#include <iomanip>
#include <time.h>

bool TriggerDriverEventLog::Set(uint32_t new_event, struct timespec new_time) {
	event = new_event;
	time = new_time;
	return true;
}

TriggerDriverEventLog::TriggerDriverEventLog(uint32_t event_type) {
	event = event_type;
	clock_gettime(CLOCK_REALTIME, &time);
}

TriggerDriverEventLog::TriggerDriverEventLog(const TriggerDriverEventLog &obj) {
	event = obj.Event();
	time = obj.Time();
}

uint32_t TriggerDriverEventLog::Event() const {
	return event;
}

struct timespec TriggerDriverEventLog::Time() const {
	return time;
}

std::ostream& operator<<(std::ostream& os, const TriggerDriverEventLog& obj) {
	std::bitset<32> t(obj.Event());
	os << t << " " << obj.Time().tv_sec << "." << std::setfill('0') << std::setw(9) << obj.Time().tv_nsec;
	return os;
}

