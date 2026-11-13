	.file	"plain.c"
	.text
	.p2align 4
	.globl	h_vec
	.type	h_vec, @function
h_vec:
.LFB1358:
	.cfi_startproc
	endbr64
	movss	(%rdi), %xmm2
	movss	4(%rdi), %xmm3
	movss	8(%rdi), %xmm1
	mulss	%xmm3, %xmm3
	movaps	%xmm2, %xmm0
	mulss	%xmm2, %xmm0
	mulss	%xmm1, %xmm1
	addss	%xmm3, %xmm0
	addss	%xmm1, %xmm0
	movss	.LC0(%rip), %xmm1
	sqrtss	%xmm0, %xmm0
	comiss	%xmm0, %xmm1
	ja	.L8
	movss	.LC2(%rip), %xmm1
	divss	%xmm0, %xmm1
	mulss	%xmm1, %xmm2
	movss	%xmm2, (%rsi)
	movss	4(%rdi), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, 4(%rsi)
	mulss	8(%rdi), %xmm1
	movss	%xmm1, 8(%rsi)
	ret
	.p2align 4,,10
	.p2align 3
.L8:
	movl	$0x00000000, 8(%rsi)
	movq	$0, (%rsi)
	ret
	.cfi_endproc
.LFE1358:
	.size	h_vec, .-h_vec
	.p2align 4
	.globl	h_mat
	.type	h_mat, @function
h_mat:
.LFB1359:
	.cfi_startproc
	endbr64
	movaps	(%rdi), %xmm9
	movaps	32(%rdi), %xmm0
	movaps	16(%rdi), %xmm2
	movaps	48(%rdi), %xmm1
	movaps	%xmm9, %xmm6
	unpcklps	%xmm0, %xmm9
	movdqa	.LC6(%rip), %xmm3
	unpckhps	%xmm0, %xmm6
	movaps	%xmm2, %xmm0
	unpcklps	%xmm1, %xmm2
	unpckhps	%xmm1, %xmm0
	movaps	%xmm6, %xmm8
	movaps	%xmm2, %xmm5
	movaps	%xmm0, %xmm7
	movaps	%xmm0, %xmm4
	movaps	%xmm0, %xmm10
	unpckhps	%xmm6, %xmm7
	unpcklps	%xmm6, %xmm4
	unpckhps	%xmm9, %xmm5
	mulps	%xmm2, %xmm6
	movaps	%xmm2, %xmm1
	movlhps	%xmm9, %xmm8
	mulps	%xmm9, %xmm0
	unpcklps	%xmm9, %xmm1
	pshufd	$136, %xmm7, %xmm12
	pshufd	$221, %xmm7, %xmm11
	movlhps	%xmm2, %xmm10
	movaps	%xmm4, %xmm13
	mulps	%xmm11, %xmm10
	mulps	%xmm12, %xmm8
	movaps	%xmm4, %xmm12
	subps	%xmm6, %xmm0
	movaps	%xmm5, %xmm6
	shufps	$136, %xmm4, %xmm6
	mulps	%xmm9, %xmm6
	movaps	%xmm5, %xmm9
	shufps	$221, %xmm4, %xmm9
	mulps	%xmm9, %xmm2
	subps	%xmm10, %xmm8
	pshufd	$27, %xmm0, %xmm9
	mulps	%xmm0, %xmm9
	subps	%xmm2, %xmm6
	pshufd	$65, %xmm9, %xmm10
	pshufd	$177, %xmm6, %xmm2
	addps	%xmm10, %xmm9
	mulps	%xmm8, %xmm2
	pshufd	$27, %xmm2, %xmm11
	addps	%xmm11, %xmm2
	pshufd	$65, %xmm2, %xmm11
	addps	%xmm11, %xmm2
	movss	.LC2(%rip), %xmm11
	shufps	$0, %xmm11, %xmm11
	subps	%xmm9, %xmm2
	divps	%xmm2, %xmm11
	mulps	%xmm11, %xmm8
	mulps	%xmm11, %xmm0
	mulps	%xmm6, %xmm11
	movaps	%xmm5, %xmm6
	pshufd	$5, %xmm8, %xmm2
	pshufd	$175, %xmm8, %xmm8
	mulps	%xmm8, %xmm13
	pshufd	$5, %xmm0, %xmm10
	pshufd	$175, %xmm0, %xmm0
	mulps	%xmm0, %xmm12
	pshufd	$5, %xmm11, %xmm9
	pshufd	$175, %xmm11, %xmm11
	mulps	%xmm2, %xmm6
	mulps	%xmm1, %xmm2
	mulps	%xmm1, %xmm0
	mulps	%xmm5, %xmm8
	mulps	%xmm11, %xmm1
	mulps	%xmm10, %xmm5
	subps	%xmm12, %xmm6
	movaps	%xmm7, %xmm12
	subps	%xmm13, %xmm2
	mulps	%xmm11, %xmm12
	movaps	%xmm7, %xmm13
	mulps	%xmm10, %xmm13
	mulps	%xmm9, %xmm7
	subps	%xmm8, %xmm0
	mulps	%xmm9, %xmm4
	subps	%xmm5, %xmm1
	addps	%xmm12, %xmm6
	movaps	.LC5(%rip), %xmm12
	addps	%xmm13, %xmm2
	addps	%xmm7, %xmm0
	addps	%xmm4, %xmm1
	xorps	%xmm12, %xmm6
	xorps	%xmm3, %xmm2
	movaps	%xmm6, (%rsi)
	xorps	%xmm12, %xmm0
	movaps	%xmm2, 16(%rsi)
	xorps	%xmm3, %xmm1
	movaps	%xmm0, 32(%rsi)
	movaps	%xmm1, 48(%rsi)
	ret
	.cfi_endproc
.LFE1359:
	.size	h_mat, .-h_mat
	.section	.rodata.cst4,"aM",@progbits,4
	.align 4
.LC0:
	.long	872415232
	.align 4
.LC2:
	.long	1065353216
	.section	.rodata.cst16,"aM",@progbits,16
	.align 16
.LC5:
	.long	0
	.long	-2147483648
	.long	0
	.long	-2147483648
	.align 16
.LC6:
	.long	-2147483648
	.long	0
	.long	-2147483648
	.long	0
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
