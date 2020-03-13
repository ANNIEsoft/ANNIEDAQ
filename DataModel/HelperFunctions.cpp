#include <HelperFunctions.h>

HelperFunctions::HelperFunctions(zmq::context_t* zmqcontext){
  context=zmqcontext;
 
}

int HelperFunctions::UpdateConnections(std::string ServiceName, zmq::socket_t* sock, std::map<std::string,Store*> &connections){
  boost::uuids::uuid m_UUID=boost::uuids::random_generator()();
  
  long msg_id=0;
  
  zmq::socket_t Ireceive (*context, ZMQ_DEALER);
  Ireceive.connect("inproc://ServiceDiscovery");
 
  zmq::message_t send(4);
  snprintf ((char *) send.data(), 4 , "%s" ,"All") ;
          
  Ireceive.send(send);
  zmq::message_t receive;
  Ireceive.recv(&receive);
  std::istringstream iss(static_cast<char*>(receive.data()));  
  int size;
  iss>>size;
  for(int i=0;i<size;i++){

    Store *service = new Store;
    zmq::message_t servicem;
    Ireceive.recv(&servicem);

    std::istringstream ss(static_cast<char*>(servicem.data()));
    service->JsonParser(ss.str());
    std::string type;
    std::string uuid;
    service->Get("msg_value",type);
    service->Get("uuid",uuid);

    if(type == ServiceName && connections.count(uuid)==0){
      connections[uuid]=service;
      std::string ip;
      std::string port;
      service->Get("ip",ip);
      service->Get("remote_port",port);
      std::string tmp="tcp://"+ ip +":"+port;
      sock->connect(tmp.c_str());
    }
    else{
      delete service;
      service=0;
    }                                           
  }

  return connections.size();
}
