#pragma once
/// \file display.h
/// Panic info display functions

#include <stdint.h>

struct cpu_state;
struct cpu_data;

namespace panicking {

class serial_port;
class symbol_table;

struct frame
{
    frame *prev;
    uintptr_t return_addr;
};

void print_header(
        serial_port &out,
        const char *message,
        const char *function,
        const char *file,
        uint64_t line);

void print_cpu(serial_port &out, cpu_data &cpu);
void print_callstack(serial_port &out, symbol_table &syms, frame const *fp);
void print_cpu_state(serial_port &out, const cpu_state &regs);
void print_user_state(serial_port &out, const cpu_state &regs);

} // namespace panicking
