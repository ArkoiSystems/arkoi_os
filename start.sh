#!/usr/bin/env sh
set -eu

KERNEL_BIN="${KERNEL_BIN:-build/ArkoiOS.bin}"

if [ ! -f "$KERNEL_BIN" ]; then
	echo "Kernel binary not found at $KERNEL_BIN"
	echo "Build first: cmake -S . -B build && cmake --build build"
	exit 1
fi

qemu-system-i386 "$@" -no-shutdown -d int -kernel "$KERNEL_BIN"