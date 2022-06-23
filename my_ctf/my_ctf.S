    .global extract

    .text

extract:
    movq    $4, %rdx
    imulq   $2, %rdx

    movq    $1, %rdi
    movq    %rdi, %rax
    lea     -8(%rbp), %rsi
    syscall

    ret