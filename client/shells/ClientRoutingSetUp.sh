#!/bin/sh
#BY gokhangobus
echo $1 | sudo -S route del default
echo $1 | sudo -S route add default gw $2 eth0
