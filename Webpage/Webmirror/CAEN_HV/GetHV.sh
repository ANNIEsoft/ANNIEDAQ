#!/bin/bash

rm *.html
wget http://192.168.163.61:8001/CAEN_HV/ -O ./tmp
cat tmp | sed s:"/CAEN_HV/":"./":g | sed s:"/\">":".html\">":g > index.html

#sed s:"Summary_Page/":"Summary_Page.html": | sed s:"Crate_Map/":"Crate_Map.html":> index.html

rm tmp
wget http://192.168.163.61:8001/CAEN_HV/Summary_Page -O ./tmp
cat tmp | sed s:"/CAEN_HV":".":g > Summary_Page.html

rm tmp
wget http://192.168.163.61:8001/CAEN_HV/Crate_Map -O ./tmp
cat tmp | sed s:"/CAEN_HV":".":g > Crate_Map.html

rm tmp
wget http://192.168.163.61:8001/CAEN_HV/Channel_Table -O ./tmp
rm tmp2
cat tmp | sed s:"/CAEN_HV":".":g > ./tmp2
rm tmp
cat tmp2 | sed s:"../flag_icon.png":"./flag_icon.png":g > ./tmp
cat tmp | sed s:"annieadmin16":"XXXXXX":g > Channel_Table.html

rm tmp
wget http://192.168.163.61:8001/CAEN_HV/Channel_Monitor -O ./tmp
cat tmp | sed s:"/CAEN_HV":".":g > Channel_Monitor.html

rm tmp
wget http://192.168.163.61:8001/CAEN_HV/Channel_Config -O ./tmp
cat tmp | sed s:"/CAEN_HV":".":g > Channel_Config.html

rm tmp
wget http://192.168.163.61:8001/CAEN_HV/Screen_Emulation -O ./tmp
cat tmp | sed s:"/CAEN_HV":".":g > Screen_Emulation.html

rm tmp
wget http://192.168.163.61:8001/CAEN_HV/Config -O ./tmp
cat tmp | sed s:"/CAEN_HV":".":g > Config.html

rm tmp
wget http://192.168.163.61:8001/CAEN_HV/Event_Log -O ./tmp
cat tmp | sed s:"/CAEN_HV":".":g > Event_Log.html

rm tmp
wget http://192.168.163.61:8001/CAEN_HV/Software_Log -O ./tmp
cat tmp | sed s:"/CAEN_HV":".":g > Software_Log.html

rm tmp
wget http://192.168.163.61:8001/CAEN_HV/Alarm_Screencap -O ./tmp
cat tmp | sed s:"/CAEN_HV":".":g > Alarm_Screencap.html

rm tmp
wget http://192.168.163.61:8001/CAEN_HV/graphs.html -O ./tmp
cat tmp | sed s:"/CAEN_HV":".":g > graphs.html

wget http://192.168.163.61:8001/CAEN_HV/voltages.png -O ./voltages.png
wget http://192.168.163.61:8001/CAEN_HV/currents.png -O ./currents.png
wget http://192.168.163.61:8001/CAEN_HV/flag_icon.png -O ./flag_icon.png

#wget http://192.168.163.61:8001/CAEN_HV/monitor_graph.js 
