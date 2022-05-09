#ifndef PIT_H
#define PIT_H

// 2022 Ian Moffett <ian@kesscoin.com>

#include <stdint.h>


// Make sure to use CLI before calling this or undefined behaviour will occur.
void pit_set_freq(uint16_t freq_hz);
void pit_init();
void pit_tick();

#endif
