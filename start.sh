#!/usr/bin/env sh
set -eu

ISO_BIN="${ISO_BIN:-build/kernel/arkoi_kernel.iso}"

if [ ! -f "$ISO_BIN" ]; then
	echo "ISO not found at $ISO_BIN"
	echo "Build first: cmake -S . -B build && cmake --build build"
	exit 1
fi

exec qemu-system-i386 "$@" -no-shutdown -cdrom "$ISO_BIN" -serial stdio
