;; 2022 Ian Moffett <ian@kesscoin.com>
bits 64
global load_pml4

load_pml4:
    mov rax, 0x000ffffffffff000
    and rdi, rax
    mov cr3, rdi
    retq
