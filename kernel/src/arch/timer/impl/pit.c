#include <arch/timer/pit.h>
#include <arch/io/io.h>
#include <util/kernflags.h>
#include <util/asm.h>
#include <util/etc.h>
#include <arch/io/legacy-pic.h>
#include <debug/log.h>

#define CHANNEL0 0x40
#define CHANNEL1 0x41
#define CHANNEL2 0x42
#define CHANNEL3 0x43

static uint16_t ticks = 0;


// 2022 Ian Moffett <ian@kesscoin.com>

void pit_set_freq(uint16_t freq_hz) {
    int divisor = 1193180 / freq_hz;
    outportb(CHANNEL3, 0x36);                       // Send command byte.
    outportb(CHANNEL0, divisor & 0xFF);             // Send low 8 bits.
    io_wait();
    outportb(CHANNEL0, divisor >> 8);      // Send high 8 bits.
}


void pit_init() {
    pit_set_freq(100);

    if (!(kern_flags & USING_APIC)) {
        outportb(PIC1_DATA, inportb(PIC1_DATA) & ~(1));
    }
}


void pit_tick() {
    ++ticks;            // Will overflow and wrap around to 0, so no need to worry to reset the value.
}


void pit_sleep(uint16_t nticks) {
    UNGLITCH_ARG(nticks);
    uint32_t eticks = ticks + nticks;
    while (ticks < eticks);
}
