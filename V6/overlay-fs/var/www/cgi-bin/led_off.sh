#!/bin/ash
PIN=$1
echo "18" > /sys/class/gpio/export
echo "out" > /sys/class/gpio/gpio1/direction
echo "1" > /sys/class/gpio/gpio18/value
echo "18" > /sys/class/gpio/unexport
