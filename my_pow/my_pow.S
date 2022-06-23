    .intel_syntax noprefix
    .global my_pow
    .text

my_pow:
    push rdi
    push rsi
    push rax
    mov rdi, offset format
    mov rsi, 1
    xor rax, rax
    call printf
    pop rax
    pop rsi
    pop rdi

    cmp rdi, 0
    je  exit
    mov rax, 1

loopy:
    add rax, rax   
    dec rdi
    push rdi
    push rsi
    push rax
    mov rdi, offset format
    mov rsi, rax
    xor rax, rax
    call printf
    pop rax
    pop rsi
    pop rdi
    cmp rdi, 0
    jne  loopy

exit:
    ret

format:
    .asciz "%d\n"