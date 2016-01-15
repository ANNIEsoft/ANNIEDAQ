#include "../Unity.cpp"

Tool* Factory(std::string tool){
Tool* ret=0;

// if (tool=="Type") tool=new Type;
if (tool=="DummyTool") ret=new DummyTool;

if (tool=="InputVariables") ret=new InputVariables;
if (tool=="Postgress") ret=new Postgress;
if (tool=="BoardReader") ret=new BoardReader;
if (tool=="Monitoring") ret=new Monitoring;
if (tool=="DataRecorder") ret=new DataRecorder;
  if (tool=="Trigger") ret=new Trigger;
return ret;
}

