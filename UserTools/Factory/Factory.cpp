#include "../Unity.cpp"

Tool* Factory(std::string tool){
Tool* ret=0;

// if (tool=="Type") tool=new Type;
if (tool=="DummyTool") ret=new DummyTool;

if (tool=="InputVariables") ret=new InputVariables;
if (tool=="Postgress") ret=new Postgress;
if (tool=="BoardReader") ret=new BoardReader;
//if (tool=="Monitoring") ret=new Monitoring;
if (tool=="DataRecorder") ret=new DataRecorder;
if (tool=="Trigger") ret=new Trigger;
if (tool=="NetworkSendData") ret=new NetworkSendData;
if (tool=="NetworkReceiveData") ret=new NetworkReceiveData;

 if (tool=="VMETriggerSend")ret=new VMETriggerSend;
 if (tool=="VMESendData")ret=new VMESendData;
 if (tool=="CardDataRecorder") ret=new CardDataRecorder;
 
return ret;
}

