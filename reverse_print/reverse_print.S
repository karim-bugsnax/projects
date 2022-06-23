    .intel_syntax noprefix
    .global reverse_print
    .text
    
reverse_print:
    mov  rax, 0
    push rdi
    call strlen
    mov  rcx, rax 
    pop  rdi

    lea  rax, [rdi + rcx + 1]
    dec  rax

reverse:
    dec  rax
    push rdi
    push rax
    push rcx
    mov  rdi, [rax]
    call putchar
    pop rcx
    pop rax
    pop rdi

    dec rcx
    jnz reverse
    ret
