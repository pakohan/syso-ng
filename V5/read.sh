#!/bin/bash
#insmod buf_threaded.ko size=$1
i=1
while [ $i -lt $2 ]
do
	./access
	let i+=1
	sleep 2
done
