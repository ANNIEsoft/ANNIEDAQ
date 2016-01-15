ToolDAQFrameworkPath=ToolDAQ/ToolDAQFramework

ZMQLib= -L ToolDAQ/zeromq-4.0.7/lib -lzmq 
ZMQInclude= -I ToolDAQ/zeromq-4.0.7/include/ 

BoostLib= -L /usr/local/lib -lboost_date_time
BoostInclude= -I /usr/local/include/

RootInclude=  -I/usr/include/root
RootLib=   -L/usr/lib64/root -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lMathCore -lThread -pthread -lm -ldl -rdynamic -pthread -m64

DataModelInclude = $(RootInclude)
DataModelLib = $(RootLib)

MyToolsInclude = $(RootInclude)
MyToolsLib = $(RootLib)  -L /usr/local/lib/ -lpqxx

all: lib/libMyTools.so lib/libToolChain.so lib/libStore.so include/Tool.h  lib/libDataModel.so RemoteControl

	g++ -g src/main.cpp -o main -I include -L lib -lStore -lMyTools -lToolChain -lDataModel -lpthread $(DataModelInclude) $(MyToolsInclude)  $(ZMQLib) $(ZMQInclude)  $(BoostLib) $(BoostInclude)


lib/libStore.so:

	cp $(ToolDAQFrameworkPath)/src/Store/Store.h include/
	g++ -g -fPIC -shared  -I inlcude $(ToolDAQFrameworkPath)/src/Store/Store.cpp -o lib/libStore.so


include/Tool.h:

	cp $(ToolDAQFrameworkPath)/src/Tool/Tool.h include/


lib/libToolChain.so: lib/libStore.so include/Tool.h lib/libDataModel.so lib/libMyTools.so

	cp $(ToolDAQFrameworkPath)/src/ToolChain/*.h include/
	g++ -g -fPIC -shared $(ToolDAQFrameworkPath)/src/ToolChain/ToolChain.cpp -I include -lpthread -L lib -lStore -lDataModel -lMyTools -o lib/libToolChain.so $(DataModelInclude) $(ZMQLib) $(ZMQInclude) $(MyToolsInclude)  


clean: 
	rm -f include/*.h
	rm -f lib/*.so
	rm -f main
	rm -f RemoteControl

lib/libDataModel.so: lib/libStore.so

	cp DataModel/DataModel.h include/
	g++ -g -fPIC -shared DataModel/DataModel.cpp -I include -L lib -lStore -o lib/libDataModel.so $(DataModelInclude) $(DataModelLib) $(ZMQLib) $(ZMQInclude)

lib/libMyTools.so: lib/libStore.so include/Tool.h lib/libDataModel.so

	cp UserTools/*.h include/
	cp UserTools/Factory/*.h include/
	g++ -g -fPIC -shared  UserTools/Factory/Factory.cpp -I include -L lib -lStore -lDataModel -o lib/libMyTools.so $(MyToolsInclude) $(MyToolsLib)  $(ZMQLib) $(ZMQInclude)

RemoteControl:
	cp $(ToolDAQFrameworkPath)/RemoteControl ./