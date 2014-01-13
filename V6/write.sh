#!/bin/bash
#insmod buf_threaded.ko size=$1
i=1
while [ $i -lt $2 ]
do
	echo "abcde" > /dev/mod_buf0
	let i+=1
	sleep 2
done
