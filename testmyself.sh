###################################
# File Name :testmyself.sh
# Author: rootkiter
# E-mail: rootkiter@rootkiter.com
# Created Time : Tue 17 Nov 2015 06:52:56 PM PST
###################################
#!/bin/sh
make
./nserver &
./nadmin  &
./nclient &
./nclient &
