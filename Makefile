ToolDAQFrameworkPath=ToolDAQ/ToolDAQFramework

ZMQLib= -L ToolDAQ/zeromq-4.0.7/lib -lzmq 
ZMQInclude= -I ToolDAQ/zeromq-4.0.7/include/ 

BoostLib= -L ToolDAQ/boost_1_60_0/install/lib -lboost_date_time -lboost_serialization -lboost_system
BoostInclude= -I ToolDAQ/boost_1_60_0/install/include

RootInclude=  -I ToolDAQ/root/include
RootLib=   -L ToolDAQ/root/lib  -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lMathCore -lThread -pthread -lm -ldl -rdynamic -pthread -m64

DataModelInclude = $(RootInclude)
DataModelLib = $(RootLib)

MyToolsInclude = $(RootInclude) -I ToolDAQ/libpqxx-4.0.1/install/include
MyToolsLib = $(RootLib)  -L ToolDAQ/libpqxx-4.0.1/install/lib/ -lpqxx  -L /usr/lib64/ -lcurl

all: lib/libMyTools.so lib/libToolChain.so lib/libStore.so include/Tool.h  lib/libServiceDiscovery.so lib/libDataModel.so lib/libLogging.so RemoteControl  NodeDaemon
 
	g++ -g src/main.cpp -o main -I include -L lib -lStore -lMyTools -lToolChain -lDataModel -lLogging -lServiceDiscovery -lpthread $(DataModelInclude) $(MyToolsInclude)  $(MyToolsLib) $(ZMQLib) $(ZMQInclude)  $(BoostLib) $(BoostInclude)


lib/libStore.so:

	cp $(ToolDAQFrameworkPath)/src/Store/Store.h include/
	g++ -g -fPIC -shared  -I include $(ToolDAQFrameworkPath)/src/Store/Store.cpp -o lib/libStore.so


include/Tool.h:

	cp $(ToolDAQFrameworkPath)/src/Tool/Tool.h include/


lib/libToolChain.so: lib/libStore.so include/Tool.h lib/libDataModel.so lib/libMyTools.so lib/libServiceDiscovery.so lib/libLogging.so

	cp $(ToolDAQFrameworkPath)/src/ToolChain/*.h include/
	g++ -g -fPIC -shared $(ToolDAQFrameworkPath)/src/ToolChain/ToolChain.cpp -I include -lpthread -L lib -lStore -lDataModel -lMyTools -lServiceDiscovery -lLogging -o lib/libToolChain.so $(DataModelInclude) $(ZMQLib) $(ZMQInclude) $(MyToolsInclude)  $(BoostLib) $(BoostInclude)


clean: 
	rm -f include/*.h
	rm -f lib/*.so
	rm -f main
	rm -f RemoteControl
	rm -f NodeDaemon

lib/libDataModel.so: lib/libStore.so lib/libLogging.so include/Camac lib/Camac

	cp DataModel/DataModel.h include/
	cp DataModel/CardData.h include/
	cp DataModel/MRDData.h include/
	cp DataModel/TriggerData.h include/
	cp DataModel/timestamp.h include/
	cp DataModel/eventlog.h include/
	g++ -g -fPIC -shared DataModel/DataModel.cpp DataModel/CardData.cpp DataModel/MRDData.cpp DataModel/TriggerData.cpp DataModel/timestamp.cc DataModel/eventlog.cc -I include -L lib -lStore  -lLogging  -o lib/libDataModel.so $(DataModelInclude) $(DataModelLib) $(ZMQLib) $(ZMQInclude)  $(BoostLib) $(BoostInclude)

lib/libMyTools.so: lib/libStore.so include/Tool.h lib/libDataModel.so lib/libLogging.so

	cp UserTools/*.h include/
	cp UserTools/Factory/*.h include/
	cp UserTools/Plots/*.h include/
	cp UserTools/Plots/OriginalPlots.h include/
	cp $(ToolDAQFrameworkPath)/src/ToolChain/*.h include/
	cp $(ToolDAQFrameworkPath)/src/ServiceDiscovery/ServiceDiscovery.h include/
	g++ -g -fPIC -shared  UserTools/Factory/Factory.cpp UserTools/Plots/OriginalPlots.cpp -I include -L lib -lStore -lDataModel -lLogging -lCC -lL3 -lL4 -lm -lxx_usb -o lib/libMyTools.so $(MyToolsInclude) $(MyToolsLib) $(DataModelInclude) $(ZMQLib) $(ZMQInclude) $(BoostLib) $(BoostInclude)

RemoteControl:
	cp $(ToolDAQFrameworkPath)/RemoteControl ./

NodeDaemon:
	cp $(ToolDAQFrameworkPath)/NodeDaemon ./

lib/libServiceDiscovery.so: lib/libStore.so
	cp $(ToolDAQFrameworkPath)/src/ServiceDiscovery/ServiceDiscovery.h include/
	g++ -g -shared -fPIC -I include $(ToolDAQFrameworkPath)/src/ServiceDiscovery/ServiceDiscovery.cpp -o lib/libServiceDiscovery.so -L lib/ -lStore  $(ZMQInclude) $(ZMQLib) $(BoostLib) $(BoostInclude)

lib/libLogging.so: lib/libStore.so
	cp $(ToolDAQFrameworkPath)/src/Logging/Logging.h include/
	g++ -g -shared -fPIC -I include $(ToolDAQFrameworkPath)/src/Logging/Logging.cpp -o lib/libLogging.so -L lib/ -lStore $(ZMQInclude) $(ZMQLib) $(BoostLib) $(BoostInclude)

update:
	cd $(ToolDAQFrameworkPath)
	git pull
	cd ../..
	git pull


include/Camac:
	cp UserTools/camacinc/CamacCrate/CamacCrate.h include/
	cp UserTools/camacinc/Lecroy3377/Lecroy3377.h include/
	cp UserTools/camacinc/Lecroy4300b/Lecroy4300b.h include/
	cp UserTools/camacinc/XXUSB/libxxusb.h include/
	cp UserTools/camacinc/XXUSB/usb.h include/

lib/Camac:
	cp UserTools/camacinc/makelib/libxx_usb.so lib/
	g++ -g -shared -fPIC UserTools/camacinc/CamacCrate/CamacCrate.cpp -I include -L lib -o lib/libCC.so
	g++ -g -shared -fPIC UserTools/camacinc/Lecroy3377/Lecroy3377.cpp -I include -L lib -o lib/libL3.so
	g++ -g -shared -fPIC UserTools/camacinc/Lecroy4300b/Lecroy4300b.cpp -I include -L lib -o lib/libL4.so
