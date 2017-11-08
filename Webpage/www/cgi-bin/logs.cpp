#include <iostream>
#include <vector>  
#include <string>  
#include <stdio.h>  
#include <stdlib.h> 
#include <sstream>
#include <fstream>
/*
#include <cgicc/CgiDefs.h> 
#include <cgicc/Cgicc.h> 
#include <cgicc/HTTPHTMLHeader.h> 
#include <cgicc/HTMLClasses.h>  
*/
#include <sys/types.h>
#include <dirent.h>

       
//       DIR *fdopendir(int fd);

using namespace std;
//using namespace cgicc;

int main (){


  /// webform
  
  // Cgicc formData;

  cout << "Content-type:text/html\r\n\r\n";
  cout<<"<html>";  

  cout<<" <head> <meta http-equiv=\"refresh\" content=\"10\"><meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\" /> <title>Logs</title></head>"; 
      
 
  cout<<"<body>"; 
  cout<<"<h1 align=\"center\">Logs</h1>" ;

  /*
    cout<<"<div align='center'><form action=\"/cgi-bin/testin.cgi\" method=\"post\" >";
    cout<<"<select name=\"UUID\">";
    cout<<"<option value=\"uuid\">[i]</option></select>"; 
    cout<<"</form>";
  */

  //DIR *opendir(const char *name);

  DIR *path = opendir("/data/logs");

  dirent *files;//=readdir(path);

  while((files=readdir(path))!=NULL){

    //  
    std::stringstream test;
    test<<files->d_name;
    //std::cout<<(test.str()=="..");

    if(!(test.str()==".." || test.str()=="." || test.str()=="README.md")){
      //       std::cout<<files->d_name<<std::endl;
      std::stringstream tmp;
      tmp<<"/data/logs/"<<files->d_name;
      std::ifstream file(tmp.str().c_str());
      std::string line;

      if(file.is_open()){
	cout<<    files->d_name;
	cout<<"<div id=\""<<files<<"\" align='left' style=\"#ccc;font:12px/13px Georgia, Garamond, Serif;overflow:scroll;border:2px solid;padding:1%;height:200px\">";
	
	while (getline(file,line)){
	  //if (line.size()>0){
	  // if (line.at(0)=='#')continue;
	  // std::string key;
	  //std::string value;
	  //std::stringstream stream(line);
	  //if(stream>>key>>value) m_variables[key]=value;
	  std::cout<<line<<"<br />"<<std::endl;
	}

      
   
      std::cout<<"</div><p>";
      file.close();
      
      std::cout<<"<script type=\"text/javascript\">";
      std::cout<<"var divid = document.getElementById(\""<<files<<"\");";
      std::cout<<"divid.scrollTop = divid.scrollHeight;";
      //std::cout<<"divid.scrollLeft = divid.scrollWidth;";
      std::cout<<"</script>";
      }

    }

  }


  /*
    cout<<"<div align='center' style=\"#ccc;font:16px/26px Georgia, Garamond, Serif;overflow:scroll;border:2px solid;padding:1%\">As you can see, once there's enough text in this box, the box will grow scroll bars... that's why we call it a scroll box! You could also place an image into the scroll box.</div>";    
  */






  cout << "</body>\n";
   
  cout << "</html>\n";
   


  return 0;
}
