    .intel_syntax noprefix
    .global main

    .text
main:
    mov     rdi, offset message
    call    puts
    mov     rax, 0
    ret

message:
    .asciz "Hello, world!"
