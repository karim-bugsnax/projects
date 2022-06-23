
        .text
        .global main
main:
    mov $2, %eax 
    cmp %eax, %edi 
    jne exit_arg

     movl $2, %eax 
    xor %esi, %esi 
    xor %edx, %edx # O_RDONLY is 0
    syscall 
    xor %edx, %edx
    cmp %edx, %eax 
    jl exit_fail # Fail ==== returned -1
    movl %eax, %edi 

read: # read(file, buf, 1)
        movl    $1, %edx    
        movl    $buf, %ecx  
        movl    %edi, %ebx    
        movl    $3, %eax    
        int     $0x80

        cmp     $1, %eax    
        jne     bye

        jmp     output


output:
        # write(STDOUT_FILENO, buf, 1)
        movl    $1, %ebx    
        movl    $4, %eax
        int     $0x80
        jmp     read

bye:

        movl    $0, %ebx
        movl    $0, %eax  
        int     $0x80
        mov $60, %rax # exit is 60 and NOT 6
        mov $0, %rdi
        syscall 

exit_arg:
    mov $60, %rax # exit is 60 and NOT 6
    mov $1, %rdi
    syscall 


exit_fail:
    mov $60, %rax
    mov $2, %rdi 
    syscall 

.data
buf:
        .byte 0