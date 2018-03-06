ToolDAQFrameworkPath=ToolDAQ/ToolDAQFramework

ZMQLib= -L ToolDAQ/zeromq-4.0.7/lib -lzmq 
ZMQInclude= -I ToolDAQ/zeromq-4.0.7/include/ 

BoostLib= -L /usr/local/lib -lboost_date_time -lboost_serialization
BoostInclude= -I /usr/local/include/

DataModelInclude = 
DataModelLib = -lvme -lrt 

MyToolsInclude = 
MyToolsLib =   -lvme -lrt  -L /usr/local/lib/ -lboost_serialization

all: lib/libMyTools.so lib/libToolChain.so lib/libStore.so include/Tool.h  lib/libServiceDiscovery.so lib/libDataModel.so lib/libLogging.so RemoteControl  NodeDaemon
 
	g++ -g src/main.cpp -o main -I include -L lib -lStore -lMyTools -lToolChain -lDataModel -lLogging -lServiceDiscovery -lpthread $(DataModelInclude) $(MyToolsInclude)  $(ZMQLib) $(ZMQInclude)  $(BoostLib) $(BoostInclude) -I annietrigger/include -L annietrigger/lib -lANNIETriggerInterface -I uc500adc/include -L uc500adc/lib -lucadc -lUC500ADCInterface
	cp uc500adc/lib/* ./lib/
	cp annietrigger/lib/* ./lib

lib/libStore.so:

	cp $(ToolDAQFrameworkPath)/src/Store/Store.h include/
	g++ -g -fPIC -shared  -I inlcude $(ToolDAQFrameworkPath)/src/Store/Store.cpp -o lib/libStore.so


include/Tool.h:

	cp $(ToolDAQFrameworkPath)/src/Tool/Tool.h include/


lib/libToolChain.so: lib/libStore.so include/Tool.h lib/libDataModel.so lib/libMyTools.so lib/libServiceDiscovery.so lib/libLogging.so

	cp $(ToolDAQFrameworkPath)/src/ToolChain/*.h include/
	g++ -g -fPIC -shared $(ToolDAQFrameworkPath)/src/ToolChain/ToolChain.cpp -I include -lpthread -L lib -lStore -lDataModel -lMyTools -lServiceDiscovery -lLogging -o lib/libToolChain.so $(DataModelInclude) $(ZMQLib) $(ZMQInclude) $(MyToolsInclude)   $(BoostLib) $(BoostInclude) -I annietrigger/include -L annietrigger/lib -lANNIETriggerInterface -I uc500adc/include -L uc500adc/lib -lucadc -lUC500ADCInterface


clean: 
	rm -f include/*.h
	rm -f lib/*.so
	rm -f main
	rm -f RemoteControl
	rm -f NodeDaemon

lib/libDataModel.so: lib/libStore.so lib/libLogging.so

	cp DataModel/DataModel.h include/
	cp DataModel/CardData.h include/
	cp DataModel/TriggerData.h include/
	cp uc500adc/include/HardwareInterface.h include/
	g++ -g -fPIC -shared DataModel/DataModel.cpp DataModel/CardData.cpp DataModel/TriggerData.cpp -I include -L lib -lStore  -lLogging  -o lib/libDataModel.so $(DataModelInclude) $(DataModelLib) $(ZMQLib) $(ZMQInclude)  $(BoostLib) $(BoostInclude) -I annietrigger/include -L annietrigger/lib -lANNIETriggerInterface -I uc500adc/include -L uc500adc/lib -lucadc -lUC500ADCInterface

lib/libMyTools.so: lib/libStore.so include/Tool.h lib/libDataModel.so lib/libLogging.so

	cp UserTools/*.h include/
	cp UserTools/Factory/*.h include/
	g++ -g -fPIC -shared  UserTools/Factory/Factory.cpp -I include -L lib -lStore -lDataModel -lLogging -o lib/libMyTools.so $(MyToolsInclude) $(MyToolsLib) $(DataModelInclude) $(ZMQLib) $(ZMQInclude) $(BoostLib) $(BoostInclude) -I annietrigger/include -L annietrigger/lib -lANNIETriggerInterface -I uc500adc/include -L uc500adc/lib -lucadc -lUC500ADCInterface

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
