#!/bin/sh
#BY gokhangobus
echo $1 | sudo -S iptables -I FORWARD -s $2 -j ACCEPT
echo $1 | sudo -S iptables -I FORWARD -d $2 -j ACCEPT
