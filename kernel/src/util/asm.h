#ifndef ASM_H
#define ASM_H

#define CLI __asm__ __volatile__("cli");
#define STI __asm__ __volatile__("sti");
#define FULL_HALT __asm__ __volatile__("cli; hlt");
#define HLT __asm__ __volatile__("hlt");


#endif
