	.file	"bits.c"
	.text
	.globl	testAndSet
	.type	testAndSet, @function
testAndSet:
.LFB0:
	.cfi_startproc
	movzwl	%di, %eax
	xorl	%edi, %esi
	sall	$16, %esi
	testl	%esi, %esi
	sete	%sil
	movzbl	%sil, %esi
	movw	$0, %di
	sall	$31, %esi
	movl	%esi, %ecx
	sarl	$15, %ecx
	sall	$16, %edx
	andl	%ecx, %edx
	notl	%ecx
	andl	%edi, %ecx
	addl	%edx, %ecx
	addl	%ecx, %eax
	ret
	.cfi_endproc
.LFE0:
	.size	testAndSet, .-testAndSet
	.globl	oneMoreThan
	.type	oneMoreThan, @function
oneMoreThan:
.LFB1:
	.cfi_startproc
	cmpl	$-2147483648, %esi
	sete	%dl
	movzbl	%dl, %edx
	notl	%esi
	leal	2(%rsi,%rdi), %eax
	orl	%eax, %edx
	sete	%al
	movzbl	%al, %eax
	ret
	.cfi_endproc
.LFE1:
	.size	oneMoreThan, .-oneMoreThan
	.globl	isTmin
	.type	isTmin, @function
isTmin:
.LFB2:
	.cfi_startproc
	movl	%edi, %eax
	addl	%eax, %eax
	sete	%dl
	testl	%edi, %edi
	setne	%al
	andl	%edx, %eax
	movzbl	%al, %eax
	ret
	.cfi_endproc
.LFE2:
	.size	isTmin, .-isTmin
	.globl	halfAdd
	.type	halfAdd, @function
halfAdd:
.LFB3:
	.cfi_startproc
	movl	%edi, %eax
	xorl	%esi, %eax
	andl	%edi, %esi
	leal	(%rsi,%rsi), %edi
	orl	%edi, %eax
	ret
	.cfi_endproc
.LFE3:
	.size	halfAdd, .-halfAdd
	.globl	sameSign
	.type	sameSign, @function
sameSign:
.LFB4:
	.cfi_startproc
	movl	%edi, %eax
	xorl	%esi, %eax
	notl	%eax
	shrl	$31, %eax
	ret
	.cfi_endproc
.LFE4:
	.size	sameSign, .-sameSign
	.globl	fullAdd
	.type	fullAdd, @function
fullAdd:
.LFB5:
	.cfi_startproc
	movl	%edi, %edx
	xorl	%esi, %edx
	andl	%esi, %edi
	leal	(%rdi,%rdi), %esi
	movl	%edx, %ecx
	xorl	%esi, %ecx
	andl	%edx, %esi
	leal	(%rsi,%rsi), %edx
	movl	%ecx, %eax
	xorl	%edx, %eax
	andl	%ecx, %edx
	addl	%edx, %edx
	xorl	%edx, %eax
	andl	$15, %eax
	ret
	.cfi_endproc
.LFE5:
	.size	fullAdd, .-fullAdd
	.globl	negate
	.type	negate, @function
negate:
.LFB6:
	.cfi_startproc
	movl	%edi, %eax
	negl	%eax
	ret
	.cfi_endproc
.LFE6:
	.size	negate, .-negate
	.globl	subOK
	.type	subOK, @function
subOK:
.LFB7:
	.cfi_startproc
	movl	%edi, %edx
	subl	%esi, %edx
	xorl	%edi, %esi
	xorl	%edx, %edi
	movl	%esi, %eax
	andl	%edi, %eax
	notl	%eax
	shrl	$31, %eax
	ret
	.cfi_endproc
.LFE7:
	.size	subOK, .-subOK
	.globl	negPerByte
	.type	negPerByte, @function
negPerByte:
.LFB8:
	.cfi_startproc
	movl	%edi, %eax
	notl	%eax
	movl	%eax, %esi
	sarl	$8, %esi
	movl	%eax, %ecx
	sarl	$16, %ecx
	sarl	$24, %eax
	negl	%edi
	addl	$1, %esi
	addl	$1, %ecx
	leal	1(%rax), %edx
	movzbl	%dil, %edi
	movzbl	%sil, %eax
	movzbl	%cl, %ecx
	sall	$8, %eax
	orl	%edi, %eax
	sall	$16, %ecx
	orl	%ecx, %eax
	movl	%eax, %edi
	movl	%edx, %eax
	sall	$24, %eax
	orl	%edi, %eax
	ret
	.cfi_endproc
.LFE8:
	.size	negPerByte, .-negPerByte
	.globl	isGreater
	.type	isGreater, @function
isGreater:
.LFB9:
	.cfi_startproc
	movl	%edi, %edx
	xorl	%esi, %edx
	sarl	$31, %edx
	notl	%edi
	movl	%edi, %ecx
	shrl	$31, %ecx
	leal	1(%rsi,%rdi), %eax
	sarl	$31, %eax
	andl	%edx, %ecx
	leal	1(%rdx), %edx
	andl	%edx, %eax
	orl	%ecx, %eax
	ret
	.cfi_endproc
.LFE9:
	.size	isGreater, .-isGreater
	.globl	zeroByte
	.type	zeroByte, @function
zeroByte:
.LFB10:
	.cfi_startproc
	movq	%rdi, %rax
	movzbl	%ah, %eax
	movl	%edi, %ecx
	sarl	$16, %ecx
	movl	%edi, %edx
	shrl	$24, %edx
	testb	%dil, %dil
	sete	%sil
	testl	%eax, %eax
	sete	%al
	orl	%esi, %eax
	testb	%cl, %cl
	sete	%cl
	testl	%edx, %edx
	sete	%dl
	orl	%ecx, %eax
	orl	%edx, %eax
	movzbl	%al, %eax
	ret
	.cfi_endproc
.LFE10:
	.size	zeroByte, .-zeroByte
	.globl	modThree
	.type	modThree, @function
modThree:
.LFB11:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	%edi, %eax
	sarl	$31, %eax
	movl	%eax, %r8d
	negl	%r8d
	xorl	%eax, %edi
	leal	(%r8,%rdi), %ecx
	movzbl	%cl, %r9d
	movzbl	%ch, %ebx
	movl	%ecx, %edx
	andl	$16711680, %edx
	movl	%edx, %esi
	sarl	$16, %esi
	sarl	$24, %ecx
	addl	%ebx, %r9d
	leal	(%rsi,%r9), %edx
	addl	%edx, %ecx
	movl	%ecx, %esi
	andl	$15, %esi
	movl	%ecx, %edi
	andl	$240, %edi
	sarl	$4, %edi
	andl	$3840, %ecx
	sarl	$8, %ecx
	addl	%esi, %edi
	leal	(%rcx,%rdi), %esi
	movl	%esi, %ecx
	andl	$7, %ecx
	andl	$56, %esi
	sarl	$2, %esi
	addl	%esi, %ecx
	movl	%ecx, %esi
	andl	$3, %esi
	movl	%ecx, %edx
	andl	$4, %edx
	sarl	$2, %edx
	addl	%esi, %edx
	andl	$8, %ecx
	sarl	$2, %ecx
	addl	%edx, %ecx
	movl	%ecx, %edx
	andl	$3, %edx
	andl	$4, %ecx
	sarl	$2, %ecx
	addl	%ecx, %edx
	cmpl	$3, %edx
	sete	%cl
	movzbl	%cl, %ecx
	jne	.L14
	movl	$2, %esi
	jmp	.L13
.L14:
	movl	$0, %esi
.L13:
	addl	%esi, %ecx
	subl	%ecx, %edx
	xorl	%eax, %edx
	leal	(%r8,%rdx), %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE11:
	.size	modThree, .-modThree
	.globl	howManyBits
	.type	howManyBits, @function
howManyBits:
.LFB12:
	.cfi_startproc
	movl	%edi, %eax
	sarl	$31, %eax
	xorl	%eax, %edi
	movl	%edi, %eax
	sarl	$16, %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	sall	$4, %eax
	leal	8(%rax), %ecx
	movl	%edi, %edx
	sarl	%cl, %edx
	testl	%edx, %edx
	setne	%dl
	movzbl	%dl, %edx
	leal	(%rax,%rdx,8), %eax
	leal	4(%rax), %ecx
	movl	%edi, %edx
	sarl	%cl, %edx
	testl	%edx, %edx
	setne	%dl
	movzbl	%dl, %edx
	leal	(%rax,%rdx,4), %eax
	leal	2(%rax), %ecx
	movl	%edi, %edx
	sarl	%cl, %edx
	testl	%edx, %edx
	setne	%dl
	movzbl	%dl, %edx
	leal	(%rax,%rdx,2), %eax
	leal	1(%rax), %ecx
	movl	%edi, %edx
	sarl	%cl, %edx
	testl	%edx, %edx
	setne	%dl
	movzbl	%dl, %edx
	leal	1(%rax,%rdx), %edx
	testl	%edi, %edi
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	ret
	.cfi_endproc
.LFE12:
	.size	howManyBits, .-howManyBits
	.globl	float_half
	.type	float_half, @function
float_half:
.LFB13:
	.cfi_startproc
	movl	%edi, %r8d
	andl	$-2147483648, %r8d
	movl	%edi, %edx
	andl	$2139095040, %edx
	movl	%edi, %ecx
	subl	%r8d, %ecx
	subl	%edx, %ecx
	movl	%ecx, %eax
	movl	%ecx, %esi
	andl	$3, %esi
	subl	$3, %esi
	sarl	$23, %edx
	cmpl	$255, %edx
	je	.L24
	testl	%edx, %edx
	jne	.L19
	testl	%esi, %esi
	jne	.L20
	leal	1(%rcx), %eax
.L20:
	sarl	%eax
	jmp	.L21
.L19:
	cmpl	$1, %edx
	jne	.L22
	testl	%esi, %esi
	jne	.L23
	leal	1(%rcx), %eax
.L23:
	addl	$8388608, %eax
	sarl	%eax
	movl	$0, %edx
	jmp	.L21
.L22:
	subl	$1, %edx
.L21:
	sall	$23, %edx
	addl	%r8d, %edx
	addl	%edx, %eax
	ret
.L24:
	movl	%edi, %eax
	ret
	.cfi_endproc
.LFE13:
	.size	float_half, .-float_half
	.globl	float_negpwr2
	.type	float_negpwr2, @function
float_negpwr2:
.LFB14:
	.cfi_startproc
	cmpl	$149, %edi
	jg	.L29
	cmpl	$126, %edi
	jle	.L27
	movl	$149, %ecx
	subl	%edi, %ecx
	movl	$1, %eax
	sall	%cl, %eax
	ret
.L27:
	movl	$127, %eax
	subl	%edi, %eax
	cmpl	$-128, %edi
	jge	.L28
	movl	$255, %eax
.L28:
	sall	$23, %eax
	ret
.L29:
	movl	$0, %eax
	ret
	.cfi_endproc
.LFE14:
	.size	float_negpwr2, .-float_negpwr2
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.4) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
