#!/bin/sh

`cat /etc/profile | grep 'ulimit'` ||  echo "ulimit -c unlimited" >> /etc/profile

if [ "$(ls /dev/ | grep -w "ttyATH0")" ] ; then
   /usr/bin/zigbeeservice /dev/ttyATH0 >> /tmp/log/run.log &
  else
  /usr/bin/zigbeeservice /dev/ttyUSB0 >> /tmp/log/run.log &
fi

sleep 1

while :
do
  stillRunning=$(ps -w | grep -w "zigbeeservice" |grep -v "grep")
  if [ "$stillRunning" ] ; then
	echo "">/dev/null
  else
    cp -f /tmp/log/run.log /tmp/log/carsh.log
    if [ "$(ls /dev/ | grep -w "ttyATH0")" ] ; then
		/usr/bin/zigbeeservice /dev/ttyATH0 >> /tmp/log/run.log &
	else
		/usr/bin/zigbeeservice /dev/ttyUSB0 >> /tmp/log/run.log &
	fi
  fi
  ls -l /tmp/log/run.log | awk '{if($5 > 1048576) system("cat /dev/null > /tmp/log/run.log")}'
  sleep 5
  ifconfig > /tmp/is_ap_connected.txt
done
