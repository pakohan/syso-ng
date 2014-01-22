#!/bin/ash
PIN=$1
#echo "18" > /sys/class/gpio/export
echo "in" > /sys/class/gpio/gpio18/direction
cat /sys/class/gpio/gpio18/value
#echo "18" > /sys/class/gpio/unexport
