## VARIAVEIS -> TABELA HASH
.data
UM:	.long	1065353216
..A: .long	 9
..H: .long	 2
.comm	..a,36
..b: .long	 1082130432
..c: .long	 2
..d: .long	 2
..e: .long	 1065353216
..f: .long	 1065353216
..g: .long	 1065353216
..h: .long	 1065353216
..i: .long	 1065353216
..j: .long	 1065353216
..k: .long	 1065353216
..I.O: .long	 1065353216
.LC0:	.string	 "%f\n"
.LC1:	.string	 "%c\n"
.LC2:	.string	 "%d\n"
.R0:	.string	 "%f"
.R1:	.string	 "%c"
.R2:	.string	 "%d"
## BEGIN FUNCTION
.globl	main
main:
	.cfi_startproc
	pushq	%rbp
##READ
	movl	$..c, %esi
	movl	$.R2, %edi
	call	__isoc99_scanf
## PRINT
	movl	..c(%rip), %eax
	movl	%eax, %esi
	movl	$.LC2, %edi
	movl	$1, %eax
	call	printf
## END FUNCTION
	popq	%rbp
	ret
	.cfi_endproc
