#ifndef MRDOUT_H
#define MRDOUT_H



class MRDOut {

  friend class boost::serialization::access;

public:
  
  MRDOut(){;}
  
  unsigned int OutN, Trigger;
  std::vector<unsigned int> Value, Slot, Channel, Crate;
  std::vector<std::string> Type;
  ULong64_t TimeStamp;
  
 private:
  
  template <class Archive> void serialize(Archive& ar,
					  const unsigned int version)
  {
    
    ar & OutN;
    ar & Trigger;
    ar & Value;
    ar & Slot;
    ar & Channel;
    ar & Crate;
    ar & Type;
    ar & TimeStamp;

  }






};

#endif
