#!/bin/sh

GPIO=$(grep -oE 'gpio-[0-9]+' /sys/kernel/debug/gpio | head -1 | cut -d- -f2)

GPIO=512

setint()
{
    case "$1" in
        falling|rising|both)
            echo $1 > /sys/class/gpio/gpio$GPIO/edge
            ;;
        *)
            echo "Usage: setint {falling|rising|both}"
            return 1
            ;;
    esac
}

pull()
{
    case "$1" in
        up)
		mode="pull-up"
            ;;
        down)
		mode="pull-down"
            ;;
        *)
            echo "Usage: pull {up|down}"
            return 1
            ;;
    esac
    echo "$mode" > /sys/devices/platform/gpio-sim.0/gpiochip0/sim_gpio0/pull
}

gpioval()
{
    cat /sys/class/gpio/gpio$GPIO/value
}

irqcnt()
{
    grep gpio /proc/interrupts
}
