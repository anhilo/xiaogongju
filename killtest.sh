###################################
# File Name :killtest.sh
# Author: rootkiter
# E-mail: rootkiter@rootkiter.com
# Created Time : Tue 17 Nov 2015 07:09:53 PM PST
###################################
#!/bin/sh
ps -a | grep nserver | awk {'print $1'} | xargs kill
ps -a | grep nclient | awk {'print $1'} | xargs kill
ps -a | grep nadmin | awk {'print $1'} | xargs kill
