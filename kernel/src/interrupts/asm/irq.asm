;; 2022 Ian Moffett <ian@kesscoin.com>
bits 64

extern pit_tick
global irq0_handler

irq0_handler:
    cli
    call pit_tick
    mov al, 0x20
    out 0x20, al
    sti
    retq
