    .intel_syntax noprefix
    .global my_pow
    .text

my_pow:
    mov ecx, 0
    mov ebx, 1
    push rcx
    push rdi
    cmp rdi, 0
    je  shite
    mov eax, ebx
    mov rdi, offset str
    mov esi, eax
    mov al, 0
    call printf
    
loopy:
    add ebx, ebx   
    mov eax, ebx
    mov rdi, offset str
    mov esi, eax
    mov al, 0
    call printf
    pop rdi
    pop rcx
    inc ecx
    push rcx
    push rdi
    cmp ecx, edi
    jne  loopy

shite:
    pop rdi
    pop rcx

str:
    .asciz "%d\n"


