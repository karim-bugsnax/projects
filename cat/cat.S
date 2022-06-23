.global main

.text
main:
	mov $2, %rax 
	cmp %rax, %rdi 
	jne exit_arg


	popq %rdi 
	popq %rdi 

	movq $2, %rax # The syscall number for open
    movq 8(%rsi), %rdi
    movq $0, %rsi
	syscall # Do the syscall
	xor %rdx, %rdx
	cmp %rdx, %rax 
	jl exit_fail 
	movq %rax, %rdi 

	movq $8, %rax
	xor %rsi, %rsi 
	movq $2, %rdx 
	syscall # Do the syscall
	xor %rdx, %rdx

	movq %rax, %r10 # Save our file length

	movq $8, %rax # lseek again
	xor %rsi, %rsi # The start of the file
	xor %rdx, %rdx # SEEK_SET is 0
	xor %rdx, %rdx

	syscall 
	movq $9, %rax # mmap's number
	xor %r9, %r9 
	movq %rdi, %r8 # The file descriptor
	movq %r10, %rsi # The file length
	movq $1, %rdx 
	movq $1, %r10 
	xor %rdi, %rdi 
	syscall 
	xor %rdx, %rdx

	movq %rsi, %rdx 
	movq %rax, %rsi #
	movq $1, %rax 
	movq $3, %rax
	movq $1, %rax 
	movq $1, %rdi 
	syscall 
	xor %rdx, %rdx
	xor %rdx, %rdx

	mov $60, %rax # exit is 60
	xor %rdi, %rdi # 0
	syscall 

exit_arg:
    mov $60, %rax # exit is 60
    mov $1, %rdi 
    syscall # Exit

exit_fail:
    mov $60, %rax
    mov $2, %rdi
    syscall