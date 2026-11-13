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
.LFB982:
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
	movss	(%rsi), %xmm0
	ret
	.cfi_endproc
.LFE982:
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
.LFB984:
	.cfi_startproc
	endbr64
	movaps	32(%rdi), %xmm6
	movaps	48(%rdi), %xmm5
	movaps	16(%rdi), %xmm2
	movaps	(%rdi), %xmm4
	pshufd	$5, %xmm6, %xmm7
	pshufd	$5, %xmm5, %xmm3
	pshufd	$187, %xmm5, %xmm1
	mulps	%xmm7, %xmm1
	pshufd	$187, %xmm6, %xmm0
	pshufd	$10, %xmm6, %xmm8
	mulps	%xmm3, %xmm0
	pshufd	$10, %xmm5, %xmm7
	pshufd	$95, %xmm5, %xmm3
	mulps	%xmm8, %xmm3
	pshufd	$1, %xmm2, %xmm10
	pshufd	$90, %xmm2, %xmm9
	pshufd	$191, %xmm2, %xmm8
	subps	%xmm0, %xmm1
	pshufd	$95, %xmm6, %xmm0
	mulps	%xmm7, %xmm0
	pshufd	$232, %xmm1, %xmm7
	mulps	%xmm9, %xmm7
	subps	%xmm0, %xmm3
	movaps	%xmm3, %xmm0
	shufps	$64, %xmm1, %xmm0
	mulps	%xmm10, %xmm0
	shufps	$173, %xmm3, %xmm1
	movaps	.LC5(%rip), %xmm3
	mulps	%xmm8, %xmm1
	subps	%xmm7, %xmm0
	addps	%xmm1, %xmm0
	xorps	%xmm3, %xmm0
	mulps	%xmm4, %xmm0
	pshufd	$177, %xmm0, %xmm1
	addps	%xmm1, %xmm0
	movhlps	%xmm0, %xmm1
	addss	%xmm1, %xmm0
	movss	.LC8(%rip), %xmm1
	andps	.LC7(%rip), %xmm0
	comiss	%xmm0, %xmm1
	ja	.L17
	movaps	%xmm2, %xmm0
	movaps	%xmm4, %xmm10
	unpcklps	%xmm5, %xmm2
	movdqa	.LC6(%rip), %xmm7
	unpckhps	%xmm5, %xmm0
	unpckhps	%xmm6, %xmm10
	unpcklps	%xmm6, %xmm4
	movaps	%xmm0, %xmm9
	movaps	%xmm10, %xmm12
	movaps	%xmm0, %xmm8
	unpckhps	%xmm10, %xmm9
	movlhps	%xmm4, %xmm12
	movlhps	%xmm2, %xmm8
	pshufd	$136, %xmm9, %xmm13
	pshufd	$221, %xmm9, %xmm11
	movaps	%xmm2, %xmm6
	mulps	%xmm11, %xmm8
	unpckhps	%xmm4, %xmm6
	movaps	%xmm0, %xmm5
	mulps	%xmm13, %xmm12
	unpcklps	%xmm10, %xmm5
	movaps	%xmm2, %xmm1
	mulps	%xmm4, %xmm0
	unpcklps	%xmm4, %xmm1
	movaps	%xmm5, %xmm13
	mulps	%xmm2, %xmm10
	subps	%xmm8, %xmm12
	movaps	%xmm6, %xmm8
	shufps	$136, %xmm5, %xmm8
	mulps	%xmm4, %xmm8
	movaps	%xmm6, %xmm4
	shufps	$221, %xmm5, %xmm4
	mulps	%xmm4, %xmm2
	subps	%xmm10, %xmm0
	pshufd	$27, %xmm0, %xmm4
	mulps	%xmm0, %xmm4
	subps	%xmm2, %xmm8
	pshufd	$177, %xmm8, %xmm2
	mulps	%xmm12, %xmm2
	pshufd	$65, %xmm4, %xmm10
	addps	%xmm10, %xmm4
	pshufd	$27, %xmm2, %xmm11
	addps	%xmm11, %xmm2
	pshufd	$65, %xmm2, %xmm11
	addps	%xmm11, %xmm2
	subps	%xmm4, %xmm2
	movss	.LC2(%rip), %xmm4
	shufps	$0, %xmm4, %xmm4
	divps	%xmm2, %xmm4
	mulps	%xmm4, %xmm12
	mulps	%xmm4, %xmm0
	mulps	%xmm8, %xmm4
	movaps	%xmm6, %xmm8
	pshufd	$5, %xmm12, %xmm2
	pshufd	$175, %xmm12, %xmm12
	mulps	%xmm2, %xmm8
	pshufd	$5, %xmm0, %xmm11
	pshufd	$175, %xmm0, %xmm0
	mulps	%xmm0, %xmm13
	pshufd	$5, %xmm4, %xmm10
	pshufd	$175, %xmm4, %xmm4
	mulps	%xmm1, %xmm2
	mulps	%xmm1, %xmm0
	mulps	%xmm4, %xmm1
	subps	%xmm13, %xmm8
	movaps	%xmm9, %xmm13
	mulps	%xmm4, %xmm13
	addps	%xmm13, %xmm8
	movaps	%xmm5, %xmm13
	mulps	%xmm12, %xmm13
	mulps	%xmm6, %xmm12
	mulps	%xmm11, %xmm6
	xorps	%xmm3, %xmm8
	mulps	%xmm10, %xmm5
	movaps	%xmm8, (%rsi)
	subps	%xmm13, %xmm2
	movaps	%xmm9, %xmm13
	mulps	%xmm11, %xmm13
	subps	%xmm12, %xmm0
	mulps	%xmm10, %xmm9
	subps	%xmm6, %xmm1
	addps	%xmm5, %xmm1
	addps	%xmm13, %xmm2
	addps	%xmm9, %xmm0
	xorps	%xmm7, %xmm1
	xorps	%xmm7, %xmm2
	movaps	%xmm1, 48(%rsi)
	xorps	%xmm3, %xmm0
	movaps	%xmm2, 16(%rsi)
	movaps	%xmm0, 32(%rsi)
.L17:
	ret
	.cfi_endproc
.LFE984:
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
	.align 16
.LC7:
	.long	2147483647
	.long	0
	.long	0
	.long	0
	.section	.rodata.cst4
	.align 4
.LC8:
	.long	925353388
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
