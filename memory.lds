
	
	OUTPUT_ARCH("riscv")

	ENTRY(_enter)

	MEMORY {
		ram : ORIGIN = 0x80000000, LENGTH = 0x4000
		flash : ORIGIN = 0x20010000, LENGTH = 0x6a120
	}

	PHDRS {
		flash PT_LOAD;
		ram PT_NULL;
	}

	SECTIONS {
		
	.init : {
		*(.init)
	} >flash AT>flash :flash

	.init_array : {
		KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)));
	} >flash AT>flash :flash

	. = ALIGN(8);
	.text : {
		*(.text .text*);
		*(.eh_frame);
	} >flash AT>flash :flash

	.rodata : {
		*(.rodata);
		*(.sdata);
	} >flash AT>flash :flash

	.bss : {
		*(.data);
		PROVIDE(__global_pointer$ = . + 0x800);
		*(.bss);
	} >ram AT>ram :ram

	.stack : {
		. = . + 0x400;
		PROVIDE(_sp = .);
	} >ram AT>ram :ram

	}

