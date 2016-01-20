#include "Postgress.h"

Postgress::Postgress():Tool(){}


bool Postgress::Initialise(std::string configfile, DataModel &data){
  
  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();
  
  m_data= &data;

  m_variables.Get("Verbose",m_verbose);

  std::stringstream tmp;
  
  
    //connect to sql
    try{
      
    tmp<<"dbname="<<*(m_variables["dbname"])<<" hostaddr="<<*(m_variables["hostaddr"])<<" port="<<*(m_variables["port"]) ;
    C=new pqxx::connection(tmp.str().c_str());
    if (C->is_open()) {
      if(m_verbose)std::cout << "Opened database successfully: " << C->dbname() << std::endl;
    } 
    else {
      std::cout << "Can't open database" << std::endl;
      return false;
    }

    tmp.str("");
    
    /* Create a non-transactional object. */
    pqxx::nontransaction N(*C);

      tmp<<"select max(runnumber) from runinformation ";
      
      
      /* Execute SQL query */
      pqxx::result R( N.exec( tmp.str().c_str() ));
      
      pqxx::result::const_iterator c = R.begin();
      
      if(c[0].is_null()) m_data->RunNumber=1;
      else if(m_data->RunNumber!=c[0].as<int>()) m_data->RunNumber=c[0].as<int>()+1;
      
      tmp.str("");
      
    
    
    tmp<<"select max(subrunnumber) from runinformation where runnumber="<<m_data->RunNumber<<";";
        
    
    /* Execute SQL query */
    pqxx::result Re( N.exec( tmp.str().c_str() ));
    
    pqxx::result::const_iterator ce = Re.begin();
    
    if(ce[0].is_null()) m_data->SubRunNumber=0;
    else m_data->SubRunNumber=ce[0].as<int>()+1;
    
    /*
    // List down all the records 
    for ( pqxx:result::const_iterator c = R.begin(); c != R.end(); ++c) {
    cout << "ID = " << c[0].as<int>() << endl;
    cout << "Name = " << c[1].as<string>() << endl;
    cout << "Age = " << c[2].as<int>() << endl;
    cout << "Address = " << c[3].as<string>() << endl;
    cout << "Salary = " << c[4].as<float>() << endl;
    */
    tmp.str("");
    tmp<<" insert into Runinformation(runnumber , subrunnumber, starttime , stoptime , runtype , runstatus , numevents ) values("<<m_data->RunNumber<<","<<m_data->SubRunNumber<<",Now(),NULL,"<<m_data->RunType<<",NULL,0);";
    
    //   pqxx::work W(*C);
    N.exec(tmp.str().c_str());
    N.commit();
    
    C->disconnect ();
  }catch (const std::exception &e){
    std::cerr << e.what() << std::endl;
    return false;
  }
  
  delete C;
  C=0;
  return true;
}


bool Postgress::Execute(){

  

  return true;
}


bool Postgress::Finalise(){
  
  std::stringstream tmp;

  try{
    tmp<<"dbname="<<*(m_variables["dbname"])<<" hostaddr="<<*(m_variables["hostaddr"])<<" port="<<*(m_variables["port"]);
    C=new pqxx::connection(tmp.str().c_str());
    if (C->is_open()) {
      if (m_verbose)std::cout << "Opened database successfully: " << C->dbname() << std::endl;
    } 
    else {
      std::cout << "Can't open database" << std::endl;
      return 1;
    }
     
    tmp.str("");

    tmp<<"update runinformation set stoptime=NOW(), numevents="<<m_data->NumEvents<<" , runstatus=0 where runnumber="<<m_data->RunNumber<<" and subrunnumber="<<m_data->SubRunNumber<<";";

     // Create a non-transactional object. 
    pqxx::nontransaction N(*C);
   

    N.exec(tmp.str().c_str());
    N.commit();
    
    C->disconnect ();
  }catch (const std::exception &e){
    std::cerr << e.what() << std::endl;
    return false;
  }
  
  delete C;
  C=0;
  
  return true;
}
