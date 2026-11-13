	.file	"asm_check.c"
	.text
	.p2align 4
	.globl	f_plain
	.type	f_plain, @function
f_plain:
.LFB981:
	.cfi_startproc
	endbr64
	movss	(%rdi), %xmm0
	movss	4(%rdi), %xmm3
	movss	8(%rdi), %xmm2
	mulss	%xmm3, %xmm3
	movaps	%xmm0, %xmm1
	mulss	%xmm0, %xmm1
	mulss	%xmm2, %xmm2
	addss	%xmm3, %xmm1
	addss	%xmm2, %xmm1
	movss	.LC1(%rip), %xmm2
	sqrtss	%xmm1, %xmm1
	comiss	%xmm1, %xmm2
	ja	.L8
	movss	.LC2(%rip), %xmm2
	divss	%xmm1, %xmm2
	mulss	%xmm2, %xmm0
	movss	%xmm0, (%rsi)
	movss	4(%rdi), %xmm1
	mulss	%xmm2, %xmm1
	movss	%xmm1, 4(%rsi)
	mulss	8(%rdi), %xmm2
	movss	%xmm2, 8(%rsi)
	ret
	.p2align 4,,10
	.p2align 3
.L8:
	movl	$0x00000000, 8(%rsi)
	pxor	%xmm0, %xmm0
	movq	$0, (%rsi)
	ret
	.cfi_endproc
.LFE981:
	.size	f_plain, .-f_plain
	.p2align 4
	.globl	f_checked
	.type	f_checked, @function
f_checked:
.LFB986:
	.cfi_startproc
	endbr64
	movss	(%rdi), %xmm0
	movss	4(%rdi), %xmm3
	movss	8(%rdi), %xmm2
	mulss	%xmm3, %xmm3
	movaps	%xmm0, %xmm1
	mulss	%xmm0, %xmm1
	mulss	%xmm2, %xmm2
	addss	%xmm3, %xmm1
	addss	%xmm2, %xmm1
	movss	.LC1(%rip), %xmm2
	sqrtss	%xmm1, %xmm1
	comiss	%xmm1, %xmm2
	ja	.L15
	movss	.LC2(%rip), %xmm2
	divss	%xmm1, %xmm2
	mulss	%xmm2, %xmm0
	movss	%xmm0, (%rsi)
	movss	4(%rdi), %xmm1
	mulss	%xmm2, %xmm1
	movss	%xmm1, 4(%rsi)
	mulss	8(%rdi), %xmm2
	movss	%xmm2, 8(%rsi)
	ret
	.p2align 4,,10
	.p2align 3
.L15:
	movl	$0x00000000, 8(%rsi)
	pxor	%xmm0, %xmm0
	movq	$0, (%rsi)
	ret
	.cfi_endproc
.LFE986:
	.size	f_checked, .-f_checked
	.p2align 4
	.globl	g_plain
	.type	g_plain, @function
g_plain:
.LFB983:
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
.LFE983:
	.size	g_plain, .-g_plain
	.p2align 4
	.globl	g_checked
	.type	g_checked, @function
g_checked:
.LFB988:
	.cfi_startproc
	endbr64
	movaps	(%rdi), %xmm6
	movaps	32(%rdi), %xmm0
	movaps	16(%rdi), %xmm2
	movaps	48(%rdi), %xmm1
	movaps	%xmm6, %xmm9
	unpcklps	%xmm0, %xmm6
	movdqa	.LC6(%rip), %xmm3
	unpckhps	%xmm0, %xmm9
	movaps	%xmm2, %xmm0
	unpcklps	%xmm1, %xmm2
	unpckhps	%xmm1, %xmm0
	movaps	%xmm2, %xmm5
	movaps	%xmm2, %xmm1
	movaps	%xmm0, %xmm7
	movaps	%xmm0, %xmm10
	unpckhps	%xmm6, %xmm5
	unpckhps	%xmm9, %xmm7
	movaps	%xmm0, %xmm4
	movlhps	%xmm2, %xmm10
	pshufd	$221, %xmm7, %xmm11
	unpcklps	%xmm9, %xmm4
	movaps	%xmm9, %xmm8
	mulps	%xmm11, %xmm10
	movaps	%xmm5, %xmm11
	unpcklps	%xmm6, %xmm1
	mulps	%xmm6, %xmm0
	movlhps	%xmm6, %xmm8
	shufps	$136, %xmm4, %xmm11
	movaps	%xmm4, %xmm13
	mulps	%xmm6, %xmm11
	movaps	%xmm5, %xmm6
	pshufd	$136, %xmm7, %xmm12
	mulps	%xmm2, %xmm9
	shufps	$221, %xmm4, %xmm6
	mulps	%xmm6, %xmm2
	mulps	%xmm12, %xmm8
	movaps	%xmm4, %xmm12
	subps	%xmm9, %xmm0
	subps	%xmm2, %xmm11
	subps	%xmm10, %xmm8
	pshufd	$27, %xmm0, %xmm6
	mulps	%xmm0, %xmm6
	pshufd	$177, %xmm11, %xmm2
	mulps	%xmm8, %xmm2
	pshufd	$65, %xmm6, %xmm9
	pshufd	$27, %xmm2, %xmm10
	addps	%xmm9, %xmm6
	addps	%xmm10, %xmm2
	pshufd	$65, %xmm2, %xmm10
	addps	%xmm10, %xmm2
	subps	%xmm6, %xmm2
	movss	.LC2(%rip), %xmm6
	shufps	$0, %xmm6, %xmm6
	divps	%xmm2, %xmm6
	mulps	%xmm6, %xmm8
	mulps	%xmm6, %xmm0
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
.LFE988:
	.size	g_checked, .-g_checked
	.section	.rodata.cst4,"aM",@progbits,4
	.align 4
.LC1:
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
