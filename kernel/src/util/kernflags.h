#ifndef KERNFLAGS_H
#define KERNFLAGS_H

// 2022 Ian Moffett <ian@kesscoin.com>

#include <stdint.h>

extern uint8_t kern_flags;

#define USING_APIC (1 << 0)
#define USING_ACPI (1 << 1)


#endif
