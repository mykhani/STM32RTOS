ENTRY(reset_handler)

MEMORY
{
	SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 96K
	FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 256K
}

SECTIONS
{
	/* output .text section */
	.text :
	{
		*(.vectors)
		*(.text)
		*(.text.*)
		*(.init)
		*(.fini)
		*(.rodata)
		*(.rodata.*)
		_etext = .;
		. = ALIGN(4);
	} > FLASH 

	_la_data = LOADADDR(.data);

	/* output .data section */
	.data :
	{
		_sdata = .;
		*(.data)
		*(.data.*)
		. = ALIGN(4);
		_edata = .;
	} > SRAM AT> FLASH

	/* output .bss section */
	.bss :
	{
		_sbss = .;
		__bss_start__ = _sbss;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
		__bss_end__ = _ebss;
		. = ALIGN(4);
		end = .;
		__end__ = end;
		
	} > SRAM
}
