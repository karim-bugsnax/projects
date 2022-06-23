    .intel_syntax noprefix
    .global my_strcpy
    .text

my_strcpy:
    mov rcx, 0

loopy:
    mov rdx, [rsi + rcx]
    mov [rdi + rcx], rdx
    cmp rdx, 0
    je  _exit
    inc rcx
    jmp loopy

_exit:
    xor rax, rax
    mov rax, rdi
    ret