#include <debug/log.h>
#include <drivers/video/FrameBuffer.h>
#include <util/asm.h>
#include <util/kernflags.h>
#include <interrupts/IDT.h>
#include <interrupts/exceptions.h>
#include <interrupts/irq.h>
#include <arch/memory/vmm.h>
#include <arch/memory/memory.h>
#include <arch/memory/gdt.h>
#include <arch/timer/pit.h>
#include <arch/io/legacy-pic.h>
#include <protection/iopl.h>
#include <firmware/acpi/acpi.h>

/*
 *  Kernel flags.
 *
 *  Bit 0: Using APIC if set, otherwise legacy PIC.
 *  Bit 1: Using ACPI.
 *
 */

uint8_t kern_flags = 0x0 | (1 << 1);


canvas_t canvas = {
    .x = 0,
    .y = 55,
    .prevX = 0
};


uint8_t gLegacyModeEnabled = 0;

void log(const char* format, STATUS status, ...) {
  va_list ptr;

  size_t args = 0;

  for (size_t i = 0; i < strlen(format); ++i) {
    if (format[i] == '%') {
      switch (format[i + 1]) {
      case 's':
      case 'd':
        ++args;
        i += 2;
        break;
      }
    }
  }

  // A hacky way to get rid of the warning: second parameter of 'va_start' not last named argument
  STATUS s = status;
  status ^= status;
  status += args;
  va_start(ptr, status);

  int color = 0xFFFFFF;

  switch (s) {
  case S_WARNING:
    color = 0x800080;
    kwrite(&canvas, "[WARNING] ", color);
    break;
  case S_INFO:
    color = 0x00FF00;
    kwrite(&canvas, "[INFO] ", color);
    break;
  case S_PANIC:
    color = 0xFF0000;
    kwrite(&canvas, "** KERNEL PANIC **\n", color);
    break;
  case S_CRITICAL:
    color = 0xFF0000;
    kwrite(&canvas, "[CRITICAL] ", color);
    break;
  }

  for (size_t i = 0; i < strlen(format); ++i) {
    if (format[i] == '%') {
      switch (format[i + 1]) {
      case 's':
        {
          const char* arg = va_arg(ptr, const char*);

          for (size_t j = 0; j < strlen(arg); ++j) {
            char terminated[2] = {arg[j], 0x0};
            kwrite(&canvas, terminated, color);
          }

          ++i;

        }

        continue;
      case 'd':
        {
          int arg = va_arg(ptr, int);

          kwrite(&canvas, dec2str(arg), color);
        }
        ++i;
        continue;
      case 'x':
        {
          int arg = va_arg(ptr, int);
          kwrite(&canvas, (char*)hex2str(arg), color);
          ++i;
          continue;
        }
      }
    }

    char terminated[2] = {format[i], 0x0};
    kwrite(&canvas, terminated, color);
  }
}


static void init(meminfo_t meminfo, void* rsdp) {
    log("Setting up Global Descriptor Table..\n", S_INFO);
    gdt_load();
    log("Setting up exceptions..\n", S_INFO);
    idt_set_vec(0x0, div0_handler, TRAP_GATE_FLAGS);
    idt_set_vec(0x1, debug_exception, TRAP_GATE_FLAGS);
    idt_set_vec(0x3, breakpoint_exception, TRAP_GATE_FLAGS);
    idt_set_vec(0x4, overflow_exception, TRAP_GATE_FLAGS);
    idt_set_vec(0x5, boundrange_exceeded_exception, TRAP_GATE_FLAGS);
    idt_set_vec(0x6, invalid_opcode_exception, TRAP_GATE_FLAGS);
    idt_set_vec(0x7, dev_not_avail_exception, TRAP_GATE_FLAGS);
    idt_set_vec(0x8, double_fault, TRAP_GATE_FLAGS);
    idt_set_vec(0xA, invalid_tss_exception, TRAP_GATE_FLAGS);
    idt_set_vec(0xB, segment_not_present, TRAP_GATE_FLAGS);
    idt_set_vec(0xC, stack_segment_fault, TRAP_GATE_FLAGS);
    idt_set_vec(0xD, general_protection_fault, TRAP_GATE_FLAGS);
    idt_set_vec(0xE, page_fault, TRAP_GATE_FLAGS);
    log("Setting up IRQs..\n", S_INFO);
    idt_set_vec(0x20, irq0_handler, INT_GATE_FLAGS);
    log("Loading IDTR with Interrupt Descriptor Table Pointer..\n", S_INFO);
    idt_install();
    log("Setting up L4 paging..\n", S_INFO);
    vmm_init(meminfo);
    log("Setting IOPL of EFLAGS to 0 (ring 0 access only).\n", S_INFO);
    zero_iopl();
    log("Setting up ACPI..\n", S_INFO);
    acpi_init(rsdp);
    kern_flags &= ~(USING_APIC);        // TODO: When implenting APIC, remove this.


    if (!(kern_flags & USING_APIC)) {
        init_pic();
        pit_init();
    }
}


int _start(framebuffer_t* lfb, psf1_font_t* font, meminfo_t meminfo, void* rsdp, uint8_t legacy_mode) {
    canvas.font = font;
    canvas.lfb = lfb;
    gLegacyModeEnabled = legacy_mode;

    log("Welcome to KessOS Paranoid Edition!\n", S_INFO);
    init(meminfo, rsdp);
    STI;

    pit_sleep(200);

    CLI;
    clearScreen(&canvas, 0x000000);
    STI;

    log("Hello, World!\n", S_INFO);

    while (1) {
        __asm__ __volatile__("hlt");
    }
}
