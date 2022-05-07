#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

// 2022 Ian Moffett <ian@kesscoin.com>


void div0_handler();
void div0_handler();
void debug_exception();
void breakpoint_exception();
void overflow_exception();
void boundrange_exceeded_exception();
void invalid_opcode_exception();
void dev_not_avail_exception();
void double_fault();
void invalid_tss_exception();
void stack_segment_fault();
void general_protection_fault();
void segment_not_present();
void page_fault();

#endif
