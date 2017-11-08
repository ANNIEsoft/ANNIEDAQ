#include <iostream>
using namespace std;
 
int main ()
{
    
  cout << "Content-type:text/html\r\n\r\n";
  cout << "<html>\n";
  cout << "<head>\n";
  cout << "<title>Hello World - First CGI Program</title>\n";
  cout << "</head>\n";
  cout << "<body>\n";
  cout << "<h2>Hello World! This is my first CGI program</h2>\n";
  cout << "</body>\n";
  cout << "</html>\n";


  cout << "<form action=\"/cgi-bin/cpp_dropdown.cgi\"                        method=\"post\" target=\"_blank\"><select name=\"dropdown\">   <option value=\"Maths\" selected>Maths</option>   <option value=\"Physics\">Physics</option></select>  First Name: <input type=\"text\" name=\"first_name\"><br /><input type=\"submit\" value=\"Submit\"/></form><form action=\"/cgi-bin/cpp_get.cgi\" method=\"post\">   First Name: <input type=\"text\" name=\"first_name\"><br />   Last Name: <input type=\"text\" name=\"last_name\" /><input type=\"submit\" value=\"Submit\" /></form> ";
   
  return 0;
}
