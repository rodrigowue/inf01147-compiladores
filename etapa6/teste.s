	.file	"teste.c"
	.globl	z
	.data
	.align 4
	.type	z, @object
	.size	z, 4
z:
	.long	1082340147
	.section	.rodata

.LC1:
	.string	"%f\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$z, %esi
	.LC0: .string	"%f"
	movl	$.LC0, %edi
	movl	$0, %eax
	call	__isoc99_scanf
	movss	z(%rip), %xmm0
	cvtss2sd	%xmm0, %xmm0
	movl	$.LC1, %edi
	movl	$1, %eax
	call	printf
	movl	$0, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.10) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
