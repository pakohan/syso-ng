#!/bin/ash
PIN=$1

echo "${PIN}" > /sys/class/gpio/export
echo "out" > /sys/class/gpio/gpio${PIN}/direction

trap "echo '18' > /sys/class/gpio/unexport" SIGINT SIGTERM

while [ -e /sys/class/gpio/gpio18 ]
do
    echo "0" > /sys/class/gpio/gpio${PIN}/value
    echo "1" > /sys/class/gpio/gpio${PIN}/value
done
