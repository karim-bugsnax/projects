    .intel_syntax noprefix
    .global my_add

    .text
my_add:
    mov     eax, 0
    add     eax, edi
    add     eax, esi
    ret