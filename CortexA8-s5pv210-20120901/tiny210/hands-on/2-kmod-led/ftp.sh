#!/bin/sh
DIR=$1
FILE=$2
ftp -i -in <<! 
open 192.168.0.201
user root root
bin
cd /tmp
lcd $DIR
put $FILE
bye
