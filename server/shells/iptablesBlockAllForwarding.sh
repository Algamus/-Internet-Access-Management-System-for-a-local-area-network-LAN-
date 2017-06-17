#!/bin/sh
#BY gokhangobus
echo $1 | sudo -S iptables -F
echo $1 | sudo -S iptables -A FORWARD -i eth0 -j DROP
