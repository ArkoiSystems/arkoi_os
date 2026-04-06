ENTRY(_start)

KERNEL_VIRTUAL_BASE  = 0xC0000000;
KERNEL_PHYSICAL_BASE = 0x00100000;

_kernel_virtual_base  = KERNEL_VIRTUAL_BASE;
_kernel_physical_base = KERNEL_PHYSICAL_BASE;

SECTIONS {
	. = KERNEL_PHYSICAL_BASE;

	.multiboot.data : ALIGN(4K) {
		*(.multiboot.data)
	}

	.bootstrap.text : ALIGN(4K) {
		*(.bootstrap.text)
	}

	. += KERNEL_VIRTUAL_BASE;
	_kernel_start = .;

	.text : AT(ADDR(.text) - KERNEL_VIRTUAL_BASE) ALIGN(4K) {
		_text_start = .;
		*(.text)
		_text_end = .;
	}

	.rodata : AT(ADDR(.rodata) - KERNEL_VIRTUAL_BASE) ALIGN(4K) {
		_rodata_start = .;
		*(.rodata)
		_rodata_end = .;
	}

	.data : AT(ADDR(.data) - KERNEL_VIRTUAL_BASE) ALIGN(4K) {
		_data_start = .;
		*(.data)
		_data_end = .;
	}

	.bss : AT(ADDR(.bss) - KERNEL_VIRTUAL_BASE) ALIGN(4K) {
		_bss_start = .;
		*(COMMON)
		*(.bss)
		_bss_end = .;
	}

	.early_heap : AT(ADDR(.early_heap) - KERNEL_VIRTUAL_BASE) ALIGN(4K) {
		_early_heap_start = .;
		. = . + 512K;
		_early_heap_end = .;
	}

	.stack (NOLOAD) : AT(ADDR(.stack) - KERNEL_VIRTUAL_BASE) ALIGN(16) {
		stack_bottom = .;
		. = . + 4K;
		stack_top = .;
	}

	_kernel_end = .;
}