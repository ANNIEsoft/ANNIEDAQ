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
      if(m_verbose)std::cout << "Opened database successfully: " << C.at(0)->dbname() << std::endl;
      //std::cout << "syncing started"<<std::endl;     
      PSQLSync();
      //std::cout << "syncing ended"<<std::endl;

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
    

    // pqxx::work W(*C);
    N.exec(tmp.str().c_str());
    N.commit();
    
   
    
    if(C.size()==2){
      PSQLSync();
      C.at(1)->disconnect ();
      delete C.at(1);
      C.at(1)=0;
    }

    C.at(0)->disconnect ();
    delete C.at(0);
    C.at(0)=0;
    
  }catch (const std::exception &e){
    std::cerr << e.what() << std::endl;
    return false;
  }
  
    C.clear();
    //delete C;
    // C=0;

    m_data->InfoTitle="PostgresVariables";
    m_variables.Set("RunNumber",m_data->RunNumber);
    m_variables.Set("SubRunNumber",m_data->SubRunNumber);
    m_variables.Set("RunType",m_data->RunType);
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    m_variables.Set("StarTime",ms); 
    m_variables>>m_data->InfoMessage;
    m_data->GetTTree("RunInformation")->Fill();
    
  return true;
}


bool Postgress::Execute(){

  if(m_data->Restart==1)Finalise();
  else if(m_data->Restart==2)Initialise("",*m_data);

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
      if(m_verbose)std::cout << "Opened database successfully: " << C.at(0)->dbname() << std::endl;

      PSQLSync();

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
    //for(int i=0;i<C.size();i++){ 
      pqxx::nontransaction N(*C.at(0));
      
      
      N.exec(tmp.str().c_str());
      N.commit();
      
      if(C.size()==2){

	pqxx::nontransaction Na(*C.at(0));
	tmp.str("");
        tmp<<"select stoptime from runinformation where runnumber="<<m_data->RunNumber<<" and subrunnumber="<<m_data->SubRunNumber<<";";
	pqxx::result Re( Na.exec( tmp.str().c_str() ));

	pqxx::result::const_iterator ce = Re.begin();

	tmp.str("");
	tmp<<"update runinformation set stoptime=( timestamp '"<<ce[0].as<std::string>()<<"'), numevents="<<m_data->NumEvents<<" , runstatus=0 where runnumber="<<m_data->RunNumber<<" and subrunnumber="<<m_data->SubRunNumber<<";";
	
	pqxx::nontransaction N2(*C.at(1));
	N2.exec(tmp.str().c_str());
	N2.commit();
   
	C.at(1)->disconnect ();
	delete C.at(1);
	C.at(1)=0;   
   }


      C.at(0)->disconnect ();
      delete C.at(0);
      C.at(0)=0;
      //}
    
  }catch (const std::exception &e){
    
    std::cerr << e.what() << std::endl;
    
    return false;
    
  }
  
  C.clear();
  // delete C;
  //C=0;
  
  return true;
}

void Postgress::PSQLSync(){

  //std::cout<<"d1"<<std::endl;

  bool pass=false;
  
  std::string query1("select count(id) from runinformation;");
  std::string query2("select runnumber, subrunnumber,starttime, stoptime, id  from runinformation order by runnumber asc, subrunnumber asc;");
  //std::cout<<"d2"<<std::endl;
  
  std::vector<pqxx::result> server1;
  std::vector<pqxx::result> server2;
  //std::cout<<"d3"<<std::endl;
  
  for(int i=0;i<C.size();i++){
    pqxx::nontransaction N(*(C.at(i)));
    //std::cout<<"d4"<<std::endl;

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

  //std::cout<<"d5"<<std::endl;  
  pqxx::result::const_iterator c1=server1.at(0).begin();
  pqxx::result::const_iterator c2=server2.at(0).begin();
  
  pass= (c1[0].as<long>()==c2[0].as<long>());
  
  //std::cout<<"d6 c1length="<<c1[0].as<long>()<<" c2length="<<c2[0].as<long>()<<std::endl;
  
  if(pass){
    //std::cout<<"d7"<<std::endl;
    // List down all the records
    //pqxx:result::const_iterator c1;
    //pqxx:result::const_iterator c2;
    
    for ( c1 = server1.at(1).begin() , c2 = server2.at(1).begin(); c1 != server1.at(1).end(); ++c1, ++c2) {
      
      pass*=(c1[0].as<long>()==c2[0].as<long>());
      pass*=(c1[1].as<long>()==c2[1].as<long>());
      pass*=(c1[2].as<std::string>()==c2[2].as<std::string>());
      if(!(c1[3].is_null()) && !(c2[3].is_null()))pass*=(c1[3].as<std::string>()==c2[3].as<std::string>());
      else if((c1[3].is_null()) && (c2[3].is_null()))pass*=true;
      else pass*=false;
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
  
  //std::cout<<"d8"<<std::endl;

  if(!pass){
    //std::cout<<"d9"<<std::endl;
    pass=true;
    bool end=false;
    for ( c1 = server1.at(1).begin() , c2 = server2.at(1).begin(); !end; ) {
      pass=true;      
      if(!(c1[0].is_null()) && !(c2[0].is_null())){  
	//std::cout<<"d9.5 "<<" c1="<<c1[0].as<long>()<<" c2="<<c2[0].as<long>()<<" , c1="<<c1[1].as<long>()<<" c2="<<c2[1].as<long>()<<std::endl;

	// std::cout<<c2[0].as<long>()<<std::endl;      
      pass*=(c1[0].as<long>()==c2[0].as<long>());
      //std::cout<<"d9.51 pass="<<pass<<std::endl;      
      pass*=(c1[1].as<long>()==c2[1].as<long>());
      //std::cout<<"d9.52 pass="<<pass<<std::endl;
      pass*=(c1[2].as<std::string>()==c2[2].as<std::string>());
      //std::cout<<"d9.53 pass="<<pass<<std::endl;
      if(!(c1[3].is_null()) && !(c2[3].is_null()))pass*=(c1[3].as<std::string>()==c2[3].as<std::string>());
      else if((c1[3].is_null()) && (c2[3].is_null()))pass*=true;
      else pass*=false;      
      //std::cout<<"d9.6"<<std::endl;
      }
      if(!pass ){
	//std::cout<<"d10 "<<" c1="<<c1[0].as<long>()<<" c2="<<c2[0].as<long>()<<" , c1="<<c1[1].as<long>()<<" c2="<<c2[1].as<long>()<<std::endl;
	if(c1[0].as<long>()<c2[0].as<long>() || (c1[1].as<long>()<c2[1].as<long>())   ){
	  
	  std::stringstream query;
	  query<<"select * from runinformation where id="<<c1[4].as<long>()<<";";
	  pqxx::nontransaction N(*(C.at(0)));
	  /* Execute SQL query */
	  pqxx::result res( N.exec( query.str().c_str() ));
	  
	  std::stringstream send;
          send<<" insert into Runinformation(id, runnumber , subrunnumber, starttime , stoptime , runtype , runstatus , numevents) values(";
	  
	  pqxx::result::const_iterator item= res.begin();
	  
	  send<<item[0];
	  for ( int i=1; i<8;i++){
	    
	    if(!(item[i].is_null())){
	      if(i==3 || i==4)send<<", TIMESTAMP '";
	      else send<<", ";
	      send<<item[i];
	      if(i==3 || i==4)send<<"'";
            }
            else send<<", NULL";

	    
	  }
	  
	  send <<");";
	  
	  pqxx::nontransaction N2(*(C.at(1)));
	  N2.exec(send.str().c_str());
	  N2.commit();
	  //insert into c2
	  //std::cout<<"insert into c2 "<<send.str()<<std::endl;

	  if(c1!=server1.at(1).end())++c1;
	  
	}
	else if( c1[0].as<long>()>c2[0].as<long>() || (c1[1].as<long>()>c2[1].as<long>())   ){
	  
	  // std::cout<<"d12"<<std::endl;
	  std::stringstream query;
	  query<<"select * from runinformation where id="<<c2[4].as<long>()<<";";
	  pqxx::nontransaction N(*(C.at(1)));
	  /* Execute SQL query */
	  pqxx::result res( N.exec( query.str().c_str() ));
	  
	  std::stringstream send;
	  send<<" insert into Runinformation(id, runnumber , subrunnumber, starttime , stoptime , runtype , runstatus , numevents) values(";
	  
	  pqxx::result::const_iterator item= res.begin();
	  
	  send<<item[0];
	  for ( int i=1; i<8;i++){
	    
	    if(!(item[i].is_null())){
	    if(i==3 || i==4)send<<", TIMESTAMP '";
            else send<<", ";
	    send<<item[i];
	    if(i==3 || i==4)send<<"'";
	    }
	    else send<<", NULL";
	  }
	  
	  send <<");";
	  
	  pqxx::nontransaction N2(*(C.at(0)));
	  N2.exec(send.str().c_str());
	  N2.commit();
	  
	  //	  insert into c1
	  //std::cout<<"insert into c1 "<<send.str()<<std::endl;

	  if(c2!=server2.at(1).end())++c2;
	  
	}
	
	else if( ((c1[3].is_null()) || (c2[3].is_null())) &&  (!((c1[3].is_null()) && (c2[3].is_null())))      ){
	  std::stringstream tmp;	  
	  if(c1[3].is_null()){

	    pqxx::nontransaction Na(*C.at(1));
	    tmp.str("");
	    tmp<<"select stoptime , numevents, runstatus from runinformation where runnumber="<<c2[0]<<" and subrunnumber="<<c2[1]<<";";
	    pqxx::result Re( Na.exec( tmp.str().c_str() ));

	    pqxx::result::const_iterator ce = Re.begin();

	    tmp.str("");
	    tmp<<"update runinformation set stoptime=( timestamp '"<<ce[0].as<std::string>()<<"'), numevents="<<ce[1]<<" , runstatus="<<ce[2]<<" where runnumber="<<c2[0]<<" and subrunnumber="<<c2[1]<<";";

	    pqxx::nontransaction N2(*C.at(0));
	    N2.exec(tmp.str().c_str());
	    N2.commit();

	  }
	  
	  else if (c2[3].is_null()){
	    

	    pqxx::nontransaction Na(*C.at(0));
            tmp.str("");
            tmp<<"select stoptime , numevents, runstatus from runinformation where runnumber="<<c1[0]<<" and subrunnumber="<<c1[1]<<";";
	    pqxx::result Re( Na.exec( tmp.str().c_str() ));

	    pqxx::result::const_iterator ce = Re.begin();

            tmp.str("");
            tmp<<"update runinformation set stoptime=( timestamp '"<<ce[0].as<std::string>()<<"'), numevents="<<ce[1]<<" , runstatus="<<ce[2]<<" where runnumber="<<c1[0]<<" and subrunnumber="<<c1[1]<<";";

	    pqxx::nontransaction N2(*C.at(1));
            N2.exec(tmp.str().c_str());
            N2.commit();



	  }
	  
	}
	
	else {
	  
	  std::cout<<"SERVER CONFLICT ERROR!!!"<<std::endl;
	  pass=true;  

	}
	
	//std::cout<<"d13"<<std::endl;
	
      }

      
      if(pass){
	//std::cout<<"d14"<<std::endl;
	if(c1!=server1.at(1).end() && c2!=server2.at(1).end()){++c1;++c2;}
	else if(c1!=server1.at(1).end() && c2==server2.at(1).end()){
	  //std::cout<<"d14a "<<c1[4].as<long>()<<std::endl;
	  std::stringstream query;
          query<<"select * from runinformation where id>="<<c1[4].as<long>()<<";";
	  pqxx::nontransaction N(*(C.at(0)));
          /* Execute SQL query */
	  pqxx::result res( N.exec( query.str().c_str() ));
	  //std::cout<<"d14a2"<<std::endl;	  
	  for( pqxx::result::const_iterator item= res.begin(); item!=res.end();item++){
	    //std::cout<<"d14a3"<<std::endl;
	    
	    std::stringstream send;
	    send<<" insert into Runinformation(id, runnumber , subrunnumber, starttime , stoptime , runtype , runstatus , numevents) values(";
	    
	    send<<item[0];
	    for ( int i=1; i<8;i++){
	      
	      if(!(item[i].is_null())){
		if(i==3 || i==4)send<<", TIMESTAMP '";
		else send<<", ";
		send<<item[i];
		if(i==3 || i==4)send<<"'";
	      }
	      else send<<", NULL";

	    }
	    
	    send <<");";
	    //std::cout<<"d14a4"<<std::endl;
	    pqxx::nontransaction N(*(C.at(1)));
	    //std::cout<<"d14a4a"<<std::endl;	    
	    N.exec(send.str().c_str());
	    //std::cout<<"d14a4b"<<std::endl;
	    N.commit();
	    //std::cout<<"insert into c2 "<<send.str()<<std::endl;
	    
	  }
	  //std::cout<<"d14a5"<<std::endl;
	  end=true;	    
	}
	else if(c2!=server2.at(1).end() && c1==server1.at(1).end()){
	  //std::cout<<"d14b"<<std::endl;
	  std::stringstream query;
          query<<"select * from runinformation where id>="<<c2[4].as<long>()<<";";
	  pqxx::nontransaction N(*(C.at(1)));
          /* Execute SQL query */
	  pqxx::result res( N.exec( query.str().c_str() ));
	  
          for( pqxx::result::const_iterator item= res.begin(); item!=res.end();item++){
	    
	    
	    std::stringstream send;
            send<<" insert into Runinformation(id, runnumber , subrunnumber, starttime , stoptime , runtype , runstatus , numevents) values(";
	    
            send<<item[0];
            for ( int i=1; i<8;i++){
	      
	      if(!(item[i].is_null())){
		if(i==3 || i==4)send<<", TIMESTAMP '";
		else send<<", ";
		send<<item[i];
		if(i==3 || i==4)send<<"'";
	      }
	      else send<<", NULL";


            }
	    
            send <<");";
	    
	    pqxx::nontransaction N(*(C.at(0)));
            N.exec(send.str().c_str());
            N.commit();
	    //std::cout<<"insert into c1 "<<send.str()<<std::endl;
          }
	  
	  end=true;
	}
	else if(c1==server1.at(1).end() && c2==server2.at(1).end()){
	  //std::cout<<"d14c"<<std::endl;
	  end=true;
	}
      }
      
      
    }
    
    //sync them
    
    
  }
  
  //std::cout<<"d15"<<std::endl;
}

