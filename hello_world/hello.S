 .global main

    .text
main:
    movq    $message, %rdi
    call    puts
    mov     $0, %rax
    ret

message:
    .asciz "Hello, world!"
