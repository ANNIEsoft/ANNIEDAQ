#include <iostream>
#include <vector>  
#include <string>  
#include <stdio.h>  
#include <stdlib.h> 

using namespace std;

int main(){

  cout << "Content-type:text/html\r\n\r\n";

  cout<<"<html>";
  cout<<"<frameset cols=\"10%,10%,*\">";
  cout<<"<frame src=\"./monitoring.cgi\">";
  cout<<"<frame src=\"./logs.cgi\">";
  cout<<" <frame src=\"./control.cgi\">";
  cout<<"</frameset>";
  cout<<"</html>";

  return 0;

}
