ENTRY(_start)

SECTIONS {
	. = 2M;

	.text BLOCK(4K) : ALIGN(4K) {
	    _text_start = .;
		KEEP(*(.multiboot))
		*(.text)
		_text_end = .;
	}

	.rodata BLOCK(4K) : ALIGN(4K) {
	    _rodata_start = .;
		*(.rodata)
		_rodata_end = .;
	}

	.data BLOCK(4K) : ALIGN(4K) {
	    _data_start = .;
		*(.data)
		_data_end = .;
	}

	.bss BLOCK(4K) : ALIGN(4K) {
	    _bss_start = .;
		*(COMMON)
		*(.bss)
		_bss_end = .;
	}

	.heap ALIGN(1M) : {
		_heap_start = .;
		. = . + 1M;
		_heap_end = .;
	}
}