#!/bin/sh
#BY gokhangobus
#echo $1 | sudo -S iptables -F
echo $1 | sudo -S iptables -D FORWARD -s $2 -j ACCEPT
echo $1 | sudo -S iptables -D FORWARD -d $2 -j ACCEPT
