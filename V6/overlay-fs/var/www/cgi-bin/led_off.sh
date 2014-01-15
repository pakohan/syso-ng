#!/bin/ash
PIN=$1
echo "17" > /sys/class/gpio/export
echo "out" > /sys/class/gpio/gpio17/direction
echo "1" > /sys/class/gpio/gpio17/value
echo "xy" > /sys/class/gpio/unexport
