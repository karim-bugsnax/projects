    .intel_syntax noprefix
    .global my_strlen

    .text
my_strlen:
    lea rax, [rdi]

strlen_loop:
    mov cl, [rax]
    cmp cl, 0 
    je  exit
    inc rax
    cmp cl, 0
    jne strlen_loop

exit:
    sub rax, rdi
    ret