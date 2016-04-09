#include "Postgress.h"

Postgress::Postgress():Tool(){}


bool Postgress::Initialise(std::string configfile, DataModel &data){
  
  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();
  
  m_data= &data;

  m_variables.Get("Verbose",m_verbose);

  int databaseconnections=0;
  std::stringstream tmp;
  
  
    /////////////////// Connect to sql ///////////////////////
    try{
      
    tmp<<"dbname="<<*(m_variables["dbname"])<<" hostaddr="<<*(m_variables["hostaddr1"])<<" port="<<*(m_variables["port"]) ;
    pqxx::connection *tmpC=new pqxx::connection(tmp.str().c_str());
    if(tmpC->is_open())C.push_back(tmpC);
    tmp.str("");
    tmp<<"dbname="<<*(m_variables["dbname"])<<" hostaddr="<<*(m_variables["hostaddr2"])<<" port="<<*(m_variables["port"]) ;
    tmpC=new pqxx::connection(tmp.str().c_str());
    if(tmpC->is_open())C.push_back(tmpC);
    /*

    if (C->is_open()) {
      if(m_verbose)std::cout << "Opened database successfully: " << C->dbname() << std::endl;
    } 
    else {
      std::cout << "Can't open database" << std::endl;
      return false;
    }
    */

    if(C.size()==2){
      if(m_verbose)std::cout << "Opened database successfully: " << C->dbname() << std::endl;
     
      PSQLSync();

    }


    else if(C.size()==1)   std::cout << "Can't open one of databases running asynchronously" << std::endl;
   
    else if(C.size()==0){
      std::cout << "Can't open databases" << std::endl;
      return false;
    }
    tmp.str("");
    
    //////////////// Find run number ///////////////
    /* Create a non-transactional object. */
   
    pqxx::nontransaction N(*(C.at(0)));

      tmp<<"select max(runnumber) from runinformation ";
      
      
      /* Execute SQL query */
      pqxx::result R( N.exec( tmp.str().c_str() ));
      
      pqxx::result::const_iterator c = R.begin();
      
      if(c[0].is_null()) m_data->RunNumber=1;
      else if(m_data->RunNumber!=c[0].as<long>()) m_data->RunNumber=c[0].as<long>()+1;
      tmp.str("");
      
    
      //////////////// Find Sub Run number ////////////////////

    
    tmp<<"select max(subrunnumber) from runinformation where runnumber="<<m_data->RunNumber<<";";
        
    
    /* Execute SQL query */
    pqxx::result Re( N.exec( tmp.str().c_str() ));
    
    pqxx::result::const_iterator ce = Re.begin();
    
    if(ce[0].is_null()) m_data->SubRunNumber=0;
    else m_data->SubRunNumber=ce[0].as<long>()+1;
    
    /*
    // List down all the records 
    for ( pqxx:result::const_iterator c = R.begin(); c != R.end(); ++c) {
    cout << "ID = " << c[0].as<int>() << endl;
    cout << "Name = " << c[1].as<string>() << endl;
    cout << "Age = " << c[2].as<int>() << endl;
    cout << "Address = " << c[3].as<string>() << endl;
    cout << "Salary = " << c[4].as<float>() << endl;
    */

    ////////////////// Insert run information ///////////////////////
    

    tmp.str("");
    tmp<<" insert into Runinformation(runnumber , subrunnumber, starttime , stoptime , runtype , runstatus , numevents ) values("<<m_data->RunNumber<<","<<m_data->SubRunNumber<<",Now(),NULL,"<<m_data->RunType<<",NULL,0);";
    

    //   pqxx::work W(*C);
    N.exec(tmp.str().c_str());
    N.commit();
    
    C.at(0)->disconnect ();

    if(C.size()==2){
      pqxx::nontransaction N2(*(C.at(1)));
      //   pqxx::work W(*C);
      N2.exec(tmp.str().c_str());
      N2.commit();

      C.at(1)->disconnect ();
    }

  }catch (const std::exception &e){
    std::cerr << e.what() << std::endl;
    return false;
  }
  
    C.clear();
    //delete C;
    // C=0;
  return true;
}


bool Postgress::Execute(){

  

  return true;
}


bool Postgress::Finalise(){
  
  std::stringstream tmp;
  
  try{
    ///////////////// Reconnect to database //////////////////
    

    tmp<<"dbname="<<*(m_variables["dbname"])<<" hostaddr="<<*(m_variables["hostaddr1"])<<" port="<<*(m_variables["port"]);
    
    pqxx::connection *tmpC=new pqxx::connection(tmp.str().c_str());
    if(tmpC->is_open())C.push_back(tmpC);

    tmp<<"dbname="<<*(m_variables["dbname"])<<" hostaddr="<<*(m_variables["hostaddr2"])<<" port="<<*(m_variables["port"]);

    tmpC=new pqxx::connection(tmp.str().c_str());
    if(tmpC->is_open())C.push_back(tmpC);


    if(C.size()==2){
      if(m_verbose)std::cout << "Opened database successfully: " << C->dbname() << std::endl;

      //PSQLSync();

    }


    else if(C.size()==1)   std::cout << "Can't open one of databases running asynchronously" << std::endl;

    else if(C.size()==0){
      std::cout << "Can't open databases" << std::endl;
      return false;
    }
    tmp.str("");

    /*
    if (C->is_open()) {

      if (m_verbose)std::cout << "Opened database successfully: " << C->dbname() << std::endl;
    } 
    
    else {
    
      std::cout << "Can't open database" << std::endl;
      
      return false;
    
    }
    */
    tmp.str("");



    //////////////// Update entry with end of run information ////////////

    tmp<<"update runinformation set stoptime=NOW(), numevents="<<m_data->NumEvents<<" , runstatus=0 where runnumber="<<m_data->RunNumber<<" and subrunnumber="<<m_data->SubRunNumber<<";";
    
     // Create a non-transactional object.
    for(int i=0;i<C.size();i++){ 
      pqxx::nontransaction N(*C.at(i));
      
      
      N.exec(tmp.str().c_str());
      N.commit();
      
      C.at(i)->disconnect ();
      
    }
    
  }catch (const std::exception &e){
    
    std::cerr << e.what() << std::endl;
    
    return false;
    
  }
  
  C.clear();
  // delete C;
  //C=0;
  
  return true;
}

void PSQLSync(){

  bool pass=false;
  
  std::string query1("select count(id) from runinformation;");
  std::string query2("select runnumber, subrunnumber,starttime, stoptime, id  from runinformation order by runnumber asc, subrunnumber asc;");
  
  std::vector<pqxx::result> server1;
  std::vector<pqxx::result> server2;
  
  for(int i=0;i<C.size();i++){
    pqxx::nontransaction N(*(C.at(i)));
    
    /* Execute SQL query */
    pqxx::result Re1( N.exec( query1.c_str() ));
    if(i==0)server1.push_back(Re1);
    if(i==1)server2.push_back(Re1);
    
    pqxx::result Re2(N.exec( query2.c_str() ));
    if(i==0)server1.push_back(Re2);
    if(i==1)server2.push_back(Re2);
    
    //    pqxx::result::const_iterator ce = Re.begin();
    
    //  if(ce[0].is_null()) m_data->SubRunNumber=0;
    // else m_data->SubRunNumber=ce[0].as<int>()+1;
  }
  
 pqxx:result::const_iterator c1=server1.at(0).begin();
 pqxx:result::const_iterator c2=server2.at(0).begin();
  
  pass= (c1[0].as<long>()==c2[0].as<long>());
  
  
  if(pass){
    // List down all the records
    //pqxx:result::const_iterator c1;
    //pqxx:result::const_iterator c2;
    
    for ( c1 = server1.at(1).begin() , c2 = server2.at(1).begin(); c1 != server1.at(1).end(); ++c1, ++c2) {
      
      pass*=(c1[0].as<long>()==c2[0].as<long>());
      pass*=(c1[1].as<long>()==c2[1].as<long>());
      pass*=(c1[2].as<string>()==c2[2].as<string>());
      pass*=(c1[3].as<string>()==c2[3].as<string>());
      if(!pass)break;
      
      /*
	cout << "ID = " << c[0].as<int>() << endl;
	cout << "Name = " << c[1].as<string>() << endl;
	cout << "Age = " << c[2].as<int>() << endl;
	cout << "Address = " << c[3].as<string>() << endl;
	cout << "Salary = " << c[4].as<float>() << endl;
      */
      

    }
  }
  
  if(!pass){
    pass=true;
    bool end=false;
    for ( c1 = server1.at(1).begin() , c2 = server2.at(1).begin(); !end; ) {
      
      pass*=(c1[0].as<long>()==c2[0].as<long>());
      pass*=(c1[1].as<long>()==c2[1].as<long>());
      pass*=(c1[2].as<string>()==c2[2].as<string>());
      pass*=(c1[3].as<string>()==c2[3].as<string>());
      if(!pass){
	if(c1[0].as<long>()<c2[0].as<long>() || (c1[1].as<long>()<c2[1].as<long>())   ){
	  
	  std::stringstream query;
	  query<<"select * from runinformation where id="<<c1[4].as<long>()<<";";
	  pqxx::nontransaction N(*(C.at(0)));
	  /* Execute SQL query */
	  pqxx::result res( N.exec( query.c_str() ));
	  
	  std::stringstream send;
          send<<" insert into Runinformation(runnumber , subrunnumber, starttime , stoptime , runtype , runstatus , numevents) values(";
	  
	pqxx:result::const_iterator item= res.begin();
	  
	  send<<item[0];
	  for ( int i=1; i<7;i++){
	    
	    send<<", "<<item[i];
	  }
	  
	  send <<");";
	  
	  pqxx::nontransaction N(*(C.at(1)));
	  N.exec(send.c_str());
	  N.commit();
	  //insert into c2
	  
	  if(c1!=server1.at(1).end())++c1;
	  
	}
	else if( c1[0].as<long>()>c2[0].as<long>() || (c1[1].as<long>()>c2[1].as<long>())   ){
	  
	  std::stringstream query;
	  query<<"select * from runinformation where id="<<c2[4].as<long>()<<";";
	  pqxx::nontransaction N(*(C.at(1)));
	  /* Execute SQL query */
	  pqxx::result res( N.exec( query.c_str() ));
	  
	  std::stringstream send;
	  send<<" insert into Runinformation(runnumber , subrunnumber, starttime , stoptime , runtype , runstatus , numevents) values(";
	  
	pqxx:result::const_iterator item= res.begin();
	  
	  send<<item[0];
	  for ( int i=1; i<7;i++){
	    
	    send<<", "<<item[i];
	  }
	  
	  send <<");";
	  
	  pqxx::nontransaction N(*(C.at(0)));
	  N.exec(send.c_str());
	  N.commit();
	  
	  //	  insert into c1
	  
	  if(c2!=server2.at(1).end())++c2;
	  
	}
	
	else {
	  
	  std::cout<<"SERVER CONFLICT ERROR!!!"<<std::endl;
	}
	
	
      }
      
      
      else{
	
	if(c1!=server1.at(1).end() && c2!=server2.at(1).end()){++c1;++c2;}
	else if(c1!=server1.at(1).end() && c2==server2.at(1).end()){
	  
	  std::stringstream query;
          query<<"select * from runinformation where id>"<<c1[4].as<long>()<<";";
	  pqxx::nontransaction N(*(C.at(0)));
          /* Execute SQL query */
	  pqxx::result res( N.exec( query.c_str() ));
	  
	  for( pqxx:result::const_iterator item= res.begin(); item!=res.end();item++){
	    
	    
	    std::stringstream send;
	    send<<" insert into Runinformation(runnumber , subrunnumber, starttime , stoptime , runtype , runstatus , numevents) values(";
	    
	    send<<item[0];
	    for ( int i=1; i<7;i++){
	      
	      send<<", "<<item[i];
	    }
	    
	    send <<");";
	    
	    pqxx::nontransaction N(*(C.at(1)));
	    N.exec(send.c_str());
	    N.commit();
	    
	  }
	  
	  end=true;	    
	}
	else if(c2!=server2.at(1).end() && c1==server1.at(1).end()){
	  
	  std::stringstream query;
          query<<"select * from runinformation where id>"<<c2[4].as<long>()<<";";
	  pqxx::nontransaction N(*(C.at(1)));
          /* Execute SQL query */
	  pqxx::result res( N.exec( query.c_str() ));
	  
          for( pqxx:result::const_iterator item= res.begin(); item!=res.end();item++){
	    
	    
	    std::stringstream send;
            send<<" insert into Runinformation(runnumber , subrunnumber, starttime , stoptime , runtype , runstatus , numevents) values(";
	    
            send<<item[0];
            for ( int i=1; i<7;i++){
	      
              send<<", "<<item[i];
            }
	    
            send <<");";
	    
	    pqxx::nontransaction N(*(C.at(0)));
            N.exec(send.c_str());
            N.commit();
	    
          }
	  
	  end=true;
	}
	else if(c1==server1.at(1).end() && c2==server2.at(1).end())end=true;
      }
      
      
    }
    
    //sync them
    
    
  }
  
  
}

