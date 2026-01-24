#!/bin/sh
sudo mkdir -p /sys/kernel/config/device-tree/overlays/exboard
sudo sh -c 'cat exboard.dtbo > /sys/kernel/config/device-tree/overlays/exboard/dtbo'
return 0
