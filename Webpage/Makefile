all:
	g++ control.cpp -o control.cgi -L /usr/local/lib -lboost_date_time -lStore -lServiceDiscovery -lcgicc  -I /usr/local/include  
	chmod a+x control.cgi
	g++ controlin.cpp -o controlin.cgi -L /usr/local/lib -lboost_date_time -lStore -lServiceDiscovery -lcgicc  -I /usr/local/include  
	chmod a+x controlin.cgi
#	g++ logs.cpp -o logs.cgi
#	chmod a+x logs.cgi
#	g++ DAQ.cpp -o DAQ.cgi
#	chmod a+x DAQ.cgi