#!/bin/ash
PIN=$1

echo "${PIN}" > /sys/class/gpio/export
echo "out" > /sys/class/gpio/gpio${PIN}/direction

trap "echo \"xy\" > /sys/class/gpio/unexport" EXIT

while true
do
    echo "0" > /sys/class/gpio/gpio${PIN}/value
    sleep 1
    echo "1" > /sys/class/gpio/gpio${PIN}/value
    sleep 1
done




