#!/usr/bin/env sh
qemu-system-i386 $1 $2 -no-shutdown -d int -kernel bin/ArkoiOS.bin &