#!/bin/bash

echo test1
cd /home/annie/ANNIEDAQ
source Setup.sh
cd -
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

########################## Lock function garentees only one instance #########################
exec 200>/home/annie/ANNIEDAQ/Webpage/Webmirror/lock
flock -n 200 || exit 1

##############################################

echo test2

cd /home/annie/ANNIEDAQ/Webpage/Webmirror/CAEN_HV
./GetHV.sh 
cd ..

rm -rf /home/annie/ANNIEDAQ/Webpage/Webmirror/logs
rm -rf /home/annie/ANNIEDAQ/Webpage/Webmirror/images
rm -rf /home/annie/ANNIEDAQ/Webpage/Webmirror/monitoringplots

for file in `ls /var/www/html/ | grep -v zip | grep -v output`
do
cp -r -L /var/www/html/$file /home/annie/ANNIEDAQ/Webpage/Webmirror/
done

cat /var/www/html/index.html | sed s:.cgi-bin/::g | sed s:.cgi:.html:g > /home/annie/ANNIEDAQ/Webpage/Webmirror/index.html
cat /var/www/html/instructions.html |  sed s:.cgi-bin/::g|sed s:.cgi:.html:g > /home/annie/ANNIEDAQ/Webpage/Webmirror/instructions.html
echo Mirrored at `date` : Note: Control interface does not work on mirrored site `/var/www/cgi-bin/control.cgi` | sed s:.cgi-bin/::g| sed s:.cgi:.html:g | sed s:'\.\./':'':g > /home/annie/ANNIEDAQ/Webpage/Webmirror/control.html
/var/www/cgi-bin/logs.cgi |  sed s:.cgi-bin/::g|sed s:.cgi:.html:g| sed s:'\.\./':'':g | sed s:/logs:./logs:g > /home/annie/ANNIEDAQ/Webpage/Webmirror/tmp
echo Mirrored at `date` > /home/annie/ANNIEDAQ/Webpage/Webmirror/logs.html
cat /home/annie/ANNIEDAQ/Webpage/Webmirror/tmp >> /home/annie/ANNIEDAQ/Webpage/Webmirror/logs.html
rm -f /home/annie/ANNIEDAQ/Webpage/Webmirror/tmp
echo Mirrored at `date` `/var/www/cgi-bin/monitoring.cgi` |  sed s:.cgi-bin/::g|sed s:.cgi:.html:g | sed s:'\.\./':'':g > /home/annie/ANNIEDAQ/Webpage/Webmirror/monitoring.html
echo Mirrored at `date` `/var/www/cgi-bin/monitoringnr.cgi` |  sed s:.cgi-bin/::g|sed s:.cgi:.html:g | sed s:'\.\./':'':g > /home/annie/ANNIEDAQ/Webpage/Webmirror/monitoringnr.html
echo Mirrored at `date` `/var/www/cgi-bin/SQL.cgi` |  sed s:.cgi-bin/::g| sed s:.cgi:.html:g | sed s:'\.\./':'':g > /home/annie/ANNIEDAQ/Webpage/Webmirror/SQL.html


echo finnished downloading trying to send
#for file in `ls /var/www/html/ -r`

LOG=/data/logs/archiver/archiver.log
PAPR=/annie/data/web/daq/
TARG=annieraw@anniegpvm02


PRINC=`cat  /home/annie/.kerberos/annieraw.principal`
KEYTAB=/home/annie/.kerberos/annieraw.keytab
#export KRB5CCNAME=/home/annie/.kerberos/krb5cc_archiver

kinit -k -t ${KEYTAB} ${PRINC}
scp -r -c blowfish -q /home/annie/ANNIEDAQ/Webpage/Webmirror/*  ${TARG}:${PAPR}/