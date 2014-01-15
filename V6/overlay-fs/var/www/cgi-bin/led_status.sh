#!/bin/ash
PIN=$1
echo "17" > /sys/class/gpio/export
echo "in" > /sys/class/gpio/gpio17/direction
cat /sys/class/gpio/gpio17/value
echo "xy" > /sys/class/gpio/unexport
