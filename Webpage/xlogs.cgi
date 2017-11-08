#!/bin/bash

echo "

<html>

<head> <meta http-equiv=\"refresh\" content=\"10\"><meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\" /> <title>Logs</title></head>

<body>

<h1 align=\"center\">Logs</h1>

"

for file in `ls /data/logs`
do
    echo "<a href=\"/logs/$file\"> $file </a>
<div id=$file align='left' style=\"#ccc;font:12px/13px Georgia, Garamond, Serif;overflow:scroll;border:2px solid;padding:1%;height:200px\">"

    tail /data/logs/$file > /tmp/$file -n 20

    while IFS='' read -r line || [[ -n "$line" ]]; do
	echo "$line <br />"
    done <  /tmp/$file

    
    #for line in `tail /data/logs/$file`
    #do
#	echo "$line <br />"
 #   done
    
    echo "</div><p>

<script type=\"text/javascript\">
var divid = document.getElementById(\"$file\");
divid.scrollTop = divid.scrollHeight;"
#divid.scrollLeft = divid.scrollWidth                     
    echo "</script>"
done


echo "

</body>

</html>

"