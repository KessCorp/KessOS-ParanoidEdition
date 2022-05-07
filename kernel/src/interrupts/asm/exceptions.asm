;; 2022 Ian Moffett <ian@kesscoin.com>

bits 64
;; TODO: When implementing tasks and processes, 
;; instead of halting the system during a panic, try to
;; terminate the thread that caused the issue.

extern clearScreen
extern log
extern hex2str
extern clearScreen
extern canvas

global div0_handler

%define S_WARNING 0
%define S_INFO 1
%define S_CRITICAL 2
%define S_PANIC 3

%macro panic 1
    cli
    
    ;; Clear the screen.
    mov rdi, canvas
    mov rsi, 0x212E52
    call clearScreen
        
    mov rdi, vector_fired_msg
    mov rsi, S_PANIC
    call log                    ;; Should log VECTOR FIRED: <vector>

    mov rdi, %1                 ;; Move first argument into RDI (vector that fired).
    call hex2str                ;; Convert it to string as hex.
    mov rdi, rax                ;; Move hexstr into RDI from RAX (where RAX is return value).
    mov rsi, -1                 ;; Just white text for the vector num.
    call log

    ;; Make newlines.
    mov rdi, newline
    mov rsi, -1
    call log

    mov rdi, newline
    mov rsi, -1
    call log

    ;; Write notice to screen.
    mov rdi, notice
    mov rsi, -1
    call log

    hlt
%endmacro



div0_handler:
    panic 0x0


error_code_refs:
    dq ec_div0


vector_fired_msg: db "VECTOR FIRED: ", 0x0
notice: db "System is now halted. When you are done debugging, push the power button.", 0xA, 0x0
newline: db 0xA, 0x0
