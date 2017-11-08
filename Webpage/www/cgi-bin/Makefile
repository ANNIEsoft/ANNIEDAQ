all:
	g++ control.cpp -o control.cgi -L /usr/local/lib -lboost_date_time -lStore -lServiceDiscovery -lcgicc  -I /usr/local/include  
	chmod a+x control.cgi
	g++ controlin.cpp -o controlin.cgi -L /usr/local/lib -lboost_date_time -lStore -lServiceDiscovery -lcgicc  -I /usr/local/include  
	chmod a+x controlin.cgi
	g++ logbook.cpp -o logbook.cgi -L /usr/local/lib  -lcgicc  -I /usr/local/include
	chmod a+x logbook.cgi
#	g++ logs.cpp -o logs.cgi
#	chmod a+x logs.cgi
#	g++ DAQ.cpp -o DAQ.cgi
#	chmod a+x DAQ.cgi
	g++ settings.cpp -o settings.cgi -L /usr/local/lib -lcgicc -I /usr/local/include
	chmod a+x settings.cgi
	g++ settingsin.cpp -o settingsin.cgi -L /usr/local/lib -lcgicc -I /usr/local/include
	chmod a+x settingsin.cgi
	g++ settingsin2.cpp -o settingsin2.cgi -L /usr/local/lib -lcgicc -I /usr/local/include
	chmod a+x settingsin2.cgi
	g++ settingsin3.cpp -o settingsin3.cgi -L /usr/local/lib -lcgicc -I /usr/local/include
	chmod a+x settingsin3.cgi