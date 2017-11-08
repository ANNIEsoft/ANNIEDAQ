#ifndef ANNIE_TRIGGERTIMESTAMP_H
#define ANNIE_TRIGGERTIMESTAMP_H

#include <stdint.h>
#include <ostream>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>


class TriggerTimeStamp {
	private:
//		friend class boost::serialization::access;
//		template<class Archive>
//		void servialize(Archive &ar, const unsigned int version) {
//			ar & triggermask;
//			ar & counter;
//		}
		uint32_t triggermask;
		uint32_t counter;
	public:
		TriggerTimeStamp(uint32_t triggermask=0, uint32_t counter=0);
		TriggerTimeStamp(const TriggerTimeStamp &obj);
		bool Set(uint32_t triggermask, uint32_t counter);
		uint32_t TriggerMask() const;
		uint32_t Counter() const;
};

std::ostream& operator<<(std::ostream& os, const TriggerTimeStamp& obj);

#endif
