#!/bin/sh
# pull.sh up|down
CHIP=gpio-sim.0
LINE=sim_gpio0
case "$1" in
  up)   echo pull-up   > /sys/devices/platform/$CHIP/gpiochip0/$LINE/pull ;;
  down) echo pull-down > /sys/devices/platform/$CHIP/gpiochip0/$LINE/pull ;;
  *) echo "usage: pull.sh up|down" ;;
esac
