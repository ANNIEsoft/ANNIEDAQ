ToolDAQFrameworkPath=ToolDAQ/ToolDAQFramework

ZMQLib= -L ToolDAQ/zeromq-4.0.7/lib -lzmq 
ZMQInclude= -I ToolDAQ/zeromq-4.0.7/include/ 

BoostLib= -L ToolDAQ/boost_1_66_0/install/lib -lboost_date_time -lboost_serialization  -lboost_iostreams 
BoostInclude= -I ToolDAQ/boost_1_66_0/install/include/ 

DataModelInclude = 
DataModelLib = -lvme -lrt 

MyToolsInclude = 
MyToolsLib =   -lvme -lrt  -L /usr/local/lib/ -lboost_serialization

all: lib/libMyTools.so lib/libToolChain.so lib/libStore.so include/Tool.h  lib/libServiceDiscovery.so lib/libDataModel.so lib/libLogging.so RemoteControl  NodeDaemon
 
	g++ -g src/main.cpp -o main -I include -L lib -lStore -lMyTools -lToolChain -lDataModel -lLogging -lServiceDiscovery -lpthread $(DataModelInclude) $(MyToolsInclude)  $(ZMQLib) $(ZMQInclude)  $(BoostLib) $(BoostInclude)


lib/libStore.so:

	cp $(ToolDAQFrameworkPath)/src/Store/Store.h include/
	g++ -g -fPIC -shared  -I inlcude $(ToolDAQFrameworkPath)/src/Store/Store.cpp -o lib/libStore.so


include/Tool.h:

	cp $(ToolDAQFrameworkPath)/src/Tool/Tool.h include/


lib/libToolChain.so: lib/libStore.so include/Tool.h lib/libDataModel.so lib/libMyTools.so lib/libServiceDiscovery.so lib/libLogging.so

	cp $(ToolDAQFrameworkPath)/src/ToolChain/*.h include/
	g++ -g -fPIC -shared $(ToolDAQFrameworkPath)/src/ToolChain/ToolChain.cpp -I include -lpthread -L lib -lStore -lDataModel -lMyTools -lServiceDiscovery -lLogging -o lib/libToolChain.so $(DataModelInclude) $(ZMQLib) $(ZMQInclude) $(MyToolsInclude)   $(BoostLib) $(BoostInclude)


clean: 
	rm -f include/*.h
	rm -f lib/*.so
	rm -f main
	rm -f RemoteControl
	rm -f NodeDaemon

lib/libDataModel.so: lib/libStore.so lib/libLogging.so

	cp DataModel/DataModel.h include/
	cp DataModel/CardData.h include/
	cp ToolDAQ/uc500adc/include/HardwareInterface.h include/
	g++ -g -fPIC -shared DataModel/DataModel.cpp DataModel/CardData.cpp -I include -L lib -lStore  -lLogging  -o lib/libDataModel.so $(DataModelInclude) $(DataModelLib) $(ZMQLib) $(ZMQInclude)  $(BoostLib) $(BoostInclude)

lib/libMyTools.so: lib/libStore.so include/Tool.h lib/libDataModel.so lib/libLogging.so

	cp UserTools/*.h include/
	cp UserTools/Factory/*.h include/
	cp ToolDAQ/uc500adc/include/CardSync.h include/
	cp ToolDAQ/uc500adc/include/trigger.h include/
	cp ToolDAQ/uc500adc/include/UC500ADCInterface.h include/
	cp ToolDAQ/uc500adc/include/ucadc.h include/
	g++ -g -fPIC -shared  UserTools/Factory/Factory.cpp ToolDAQ/uc500adc/src/UC500ADCInterface.cc ToolDAQ/uc500adc/src/ucadc.cc ToolDAQ/uc500adc/src/trigger.cc ToolDAQ/uc500adc/src/CardSync.cc -I include -L lib -lStore -lDataModel -lLogging -o lib/libMyTools.so $(MyToolsInclude) $(MyToolsLib) $(DataModelInclude) $(ZMQLib) $(ZMQInclude) $(BoostLib) $(BoostInclude)

RemoteControl:
	cp $(ToolDAQFrameworkPath)/RemoteControl ./

NodeDaemon:
	cp $(ToolDAQFrameworkPath)/NodeDaemon ./

lib/libServiceDiscovery.so: lib/libStore.so
	cp $(ToolDAQFrameworkPath)/src/ServiceDiscovery/ServiceDiscovery.h include/
	g++ -shared -fPIC -I include $(ToolDAQFrameworkPath)/src/ServiceDiscovery/ServiceDiscovery.cpp -o lib/libServiceDiscovery.so -L lib/ -lStore  $(ZMQInclude) $(ZMQLib) $(BoostLib) $(BoostInclude)

lib/libLogging.so: lib/libStore.so
	cp $(ToolDAQFrameworkPath)/src/Logging/Logging.h include/
	g++ -shared -fPIC -I include $(ToolDAQFrameworkPath)/src/Logging/Logging.cpp -o lib/libLogging.so -L lib/ -lStore $(ZMQInclude) $(ZMQLib) $(BoostLib) $(BoostInclude)

update:
	cd $(ToolDAQFrameworkPath)
	git pull
	cd ../..
	git pull
