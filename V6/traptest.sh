#!/bin/bash

PIN=$1

#echo "${PIN}" > /sys/class/gpio/export
#echo "out" > /sys/class/gpio/gpio${PIN}/direction
trap "trap" EXIT

while true
do
    echo "0"
    sleep 1
    echo "1"
    sleep 1
done



