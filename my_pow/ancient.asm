cmp     edi, 0
    jle     retu
    mov     rax, 1
    push    rax
    mov     rbx, 0
    mov     ecx, edi
    .loopy:
    move    rbx, 0
    call    printf
    cmp     ecx, 0
    jle     retu
    mul     rax, 2
    dec     ecx
    cmp     ecx, 0
    jle     loopy
    .retu:
    ret