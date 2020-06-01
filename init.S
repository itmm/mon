
	
	.section .init
	.global _enter
	_enter:
		
	.option push
	.option norelax
		la gp, __global_pointer$
	.option pop

    la t0, early_trap_vector
    csrw mtvec, t0

	la sp, _sp
	andi sp, sp, -16

		csrr a0, mhartid
	1:
		bnez a0, 1b

	call main

	1:
		j 1b


	.align 2
	early_trap_vector:
		j early_trap_vector

