;; 2022 Ian Moffett <ian@kesscoin.com>
bits 64

global zero_iopl

;; Will set IOPL to zero so only kernelspace can use IO ops.
zero_iopl:
    pushf                   ;; Push FLAGS onto stack.
    pop rax                 ;; Pop FLAGS from stack into RAX.
    and rax, ~(1 << 12)     ;; Zero bit 12 of FLAGS (bit 1 of IOPL).
    and rax, ~(1 << 13)     ;; Zero bit 13 of FLAGS (bit 2 of IOPL).
    push rax                ;; Push RAX onto stack.
    popf                    ;; Pop FLAGS back in place.
    retq                    ;; Done, so return. 
