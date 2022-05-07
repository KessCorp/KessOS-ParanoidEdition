;; 2022 Ian Moffett <ian@kesscoin.com>
bits 64

global gdt_load


GDT:
    .null: equ $ - GDT
        dd 0x0
        dd 0x0
    .code: equ $ - GDT
        dw 0                    ;; Limit low.
        dw 0                    ;; Base low.
        db 0                    ;; Base middle.
        db 0b10011010           ;; Access.
        db 0b10101111           ;; Granularity.
        db 0                    ;; Base high.
    .data: equ $ - GDT
        dw 0                    ;; Limit low.
        dw 0                    ;; Base low.
        db 0                    ;; Base middle.
        db 0b10010010           ;; Access.
        db 0b00000000           ;; Granularity.
        db 0                    ;; Base high.
    .user_code: equ $ - GDT
        dw 0                    ;; Limit low.
        dw 0                    ;; Base low.
        db 0                    ;; Base middle.
        db 0b11111010           ;; Access.  
        db 0b10101111           ;; Granularity.
        db 0                    ;; Base high.
    .user_data: equ $ - GDT
        dw 0                    ;; Limit low.
        dw 0                    ;; Base low.
        db 0                    ;; Base middle.
        db 0b11110010           ;; Access.
        db 0b00000000           ;; Granularity.
        db 0                    ;; Base high.
    TSS:
        dq 0
        dq 0
    Pointer:
        dw $ - GDT - 1          ;; Limit.
        dq GDT                  ;; Base.


gdt_load:
    cli                 ;; Clear interrupts to prevent funky things from happening.
    lgdt [Pointer]      ;; Load GDT into GDT register.
    mov rax, 0x10       ;; Data selector.
    mov ds, rax         ;; Set data segment to data selector.
    mov es, rax         ;; Set extra segment to data selector.
    mov ss, rax         ;; Set stack segment to data selector.
    mov fs, rax         ;; Set FS to data selector as it is currently unused.
    mov gs, rax         ;; Set GS to data selector as it is currently unused.
    pop rdi             ;; Pop return address off stack.
    mov rax, 0x08       ;; Set RAX to code selector.
    push rax            ;; Push RAX (this will be the code segment we want to use).
    push rdi            ;; Push return address.
    retfq               ;; Perform a 64 bit far return that changes CS to what is on stack before return address (value of RAX).
