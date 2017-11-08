#ifndef ANNIE_TRIGGEREVENTLOG_H
#define ANNIE_TRIGGEREVENTLOG_H

#include <stdint.h>
#include <ostream>
#include <time.h>

//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>

#define TDEL_START	1
#define TDEL_INITIALISE	2

// serializer for timespec
//namespace boost {
//namespace serialization {
//template<class Archive>
//void serialize(Archive & ar, timespec & t, const unsigned int version) {
//	ar & t.tv_sec;
//	ar & t.tv_nsec;
//}
//
//} // namespace serialization
//} // namespace boost

class TriggerDriverEventLog {
	private:
//		friend class boost::serialization::access;
//		template<class Archive>
//		void serialize(Archive & ar, const unsigned int version) {
//			ar & event;
//			ar & time;
//		}
		uint32_t event;
		timespec time;
	public:
		TriggerDriverEventLog(uint32_t event_type=0);
		TriggerDriverEventLog(const TriggerDriverEventLog &obj);
		bool Set(uint32_t new_event, struct timespec new_time);
		uint32_t Event() const;
		struct timespec Time() const;
};

std::ostream& operator<<(std::ostream& os, const TriggerDriverEventLog& obj);


#endif
