#include "InputVariables.h"

InputVariables::InputVariables():Tool(){}


bool InputVariables::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_variables.Get("RunType",m_data->RunType);

  TTree *tree=new TTree("RunInformation","RunInformation");
  TBranch *br;

  br=tree->Branch("InfoTitle",&(m_data->InfoTitle));
  br->SetCompressionLevel(1);
  br=tree->Branch("InfoMessage",&(m_data->InfoMessage));
  br->SetCompressionLevel(1);  
  m_data->AddTTree("RunInformation",tree);

  m_data->InfoTitle="ToolChainVariables";
  m_data->vars>>m_data->InfoMessage;
  m_data->GetTTree("RunInformation")->Fill();

  m_data->InfoTitle="InputVariables";
  m_variables>>m_data->InfoMessage;
  m_data->GetTTree("RunInformation")->Fill();
  
  return true;
}


bool InputVariables::Execute(){

  return true;
}


bool InputVariables::Finalise(){

  return true;
}
