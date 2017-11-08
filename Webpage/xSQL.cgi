#!/bin/bash

echo "

<html> 
<head> <meta http-equiv=\"refresh\" content=\"600\"><meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\" /> <title>SQL DataBase</title></head> 
<body>                                                                                                                                                                                                                                                                                                                                                                                                                           
<h1 align=\"center\">SQL DataBase</h1>                                                                                                                                                                                     
"          


echo "Select * from runinformation" | psql annie |sed s:" ":"~":g > /tmp/sql

while IFS='' read -r line || [[ -n "$line" ]]; do
 echo "$line <br />"
done <  /tmp/sql


echo "                                                                                                                                                                                                             
                                                                                                                                                                                                                   
</body>                                                                                                                                                                                                            
                                                                                                                                                                                                                   
</html>                                                                                                                                                                                                            
                                                                                                                                                                                                                   
"
