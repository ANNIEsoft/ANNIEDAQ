#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <iostream>
#include <zmq.hpp>
#include <sstream>
#include <pthread.h>
#include <map>
#include <Store.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.


class HelperFunctions{

 public:
  HelperFunctions(zmq::context_t* zmqcontext);
  int UpdateConnections(std::string ServiceName, zmq::socket_t* sock, std::map<std::string,Store*> &connections);

 private:

  zmq::context_t *context;

};
#endif
