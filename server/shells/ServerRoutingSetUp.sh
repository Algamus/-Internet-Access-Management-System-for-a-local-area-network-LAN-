#!/bin/sh
#BY gokhangobus
echo $1 | sudo -S sed -i 's/#net.ipv4.ip_forward=1/net.ipv4.ip_forward=1\nnet.ipv4.conf.eth0.send_redirects=0/' /etc/sysctl.conf
echo $1 | sudo -S iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE


