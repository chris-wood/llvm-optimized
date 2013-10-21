	.file	"mod_inverse.profile.bc"
	.text
	.globl	mul_inv
	.align	16, 0x90
	.type	mul_inv,@function
mul_inv:                                # @mul_inv
# BB#0:                                 # %entry
	pushl	%ebp
	movl	%esp, %ebp
	subl	$32, %esp
	incl	EdgeProfCounters
	movl	8(%ebp), %eax
	movl	%eax, -8(%ebp)
	movl	12(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	%eax, -16(%ebp)
	movl	$0, -28(%ebp)
	movl	$1, -32(%ebp)
	cmpl	$1, -12(%ebp)
	jne	.LBB0_2
# BB#1:                                 # %if.then
	incl	EdgeProfCounters+4
	movl	$1, -4(%ebp)
	incl	EdgeProfCounters+12
	jmp	.LBB0_9
.LBB0_2:                                # %if.end
	incl	EdgeProfCounters+8
	incl	EdgeProfCounters+16
	jmp	.LBB0_3
	.align	16, 0x90
.LBB0_4:                                # %while.body
                                        #   in Loop: Header=BB0_3 Depth=1
	incl	EdgeProfCounters+20
	movl	-8(%ebp), %eax
	cltd
	idivl	-12(%ebp)
	movl	%eax, -24(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, -20(%ebp)
	movl	-8(%ebp), %eax
	cltd
	idivl	-12(%ebp)
	movl	%edx, -12(%ebp)
	movl	-20(%ebp), %eax
	movl	%eax, -8(%ebp)
	movl	-28(%ebp), %eax
	movl	%eax, -20(%ebp)
	movl	-24(%ebp), %eax
	imull	-28(%ebp), %eax
	movl	-32(%ebp), %ecx
	subl	%eax, %ecx
	movl	%ecx, -28(%ebp)
	movl	-20(%ebp), %eax
	movl	%eax, -32(%ebp)
	incl	EdgeProfCounters+28
.LBB0_3:                                # %while.cond
                                        # =>This Inner Loop Header: Depth=1
	cmpl	$2, -8(%ebp)
	jge	.LBB0_4
# BB#5:                                 # %while.end
	incl	EdgeProfCounters+24
	cmpl	$0, -32(%ebp)
	js	.LBB0_7
# BB#6:                                 # %while.end.if.end4_crit_edge
	incl	EdgeProfCounters+36
	jmp	.LBB0_8
.LBB0_7:                                # %if.then3
	incl	EdgeProfCounters+32
	movl	-16(%ebp), %eax
	addl	%eax, -32(%ebp)
	incl	EdgeProfCounters+40
.LBB0_8:                                # %if.end4
	movl	-32(%ebp), %eax
	movl	%eax, -4(%ebp)
	incl	EdgeProfCounters+44
.LBB0_9:                                # %return
	movl	-4(%ebp), %eax
	addl	$32, %esp
	popl	%ebp
	ret
.Ltmp0:
	.size	mul_inv, .Ltmp0-mul_inv

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
# BB#0:                                 # %entry
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$13, 12(%esp)
	movl	$EdgeProfCounters, 8(%esp)
	movl	$0, 4(%esp)
	movl	$0, (%esp)
	calll	llvm_start_edge_profiling
	incl	EdgeProfCounters+48
	movl	$0, -4(%ebp)
	movl	$2017, 4(%esp)          # imm = 0x7E1
	movl	$42, (%esp)
	calll	mul_inv
	movl	%eax, 4(%esp)
	movl	$.L.str, (%esp)
	calll	printf
	xorl	%eax, %eax
	addl	$24, %esp
	popl	%ebp
	ret
.Ltmp1:
	.size	main, .Ltmp1-main

	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	 "%d\n"
	.size	.L.str, 4

	.type	EdgeProfCounters,@object # @EdgeProfCounters
	.local	EdgeProfCounters
	.comm	EdgeProfCounters,52,16

	.section	".note.GNU-stack","",@progbits
