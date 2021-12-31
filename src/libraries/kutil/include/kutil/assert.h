#pragma once

#include <stdint.h>

namespace kutil {
namespace assert {

constexpr uint32_t send_nmi_command =
    (4 << 8) |  // Delivery mode NMI
    (1 << 14) | // assert level high
    (1 << 18);  // destination self

extern uint32_t *apic_icr;
extern uintptr_t symbol_table;

} // namespace assert
} // namespace kutil

__attribute__ ((always_inline))
inline void kassert(
        bool check,
        const char *message = nullptr,
        const char *function = __builtin_FUNCTION(),
        const char *file = __builtin_FILE(),
        uint64_t line = __builtin_LINE())
{
    if (!check) {
        register uintptr_t syms asm("rdi");
        register const char *m asm("rsi");
        register const char *fn asm("rdx");
        register const char *fi asm("rcx");
        register uint64_t l asm("r8");

        asm volatile ("mov %1, %0" : "=r"(syms) : "r"(kutil::assert::symbol_table));
        asm volatile ("mov %1, %0" : "=r"(m) : "r"(message));
        asm volatile ("mov %1, %0" : "=r"(fn) : "r"(function));
        asm volatile ("mov %1, %0" : "=r"(fi) : "r"(file));
        asm volatile ("mov %1, %0" : "=r"(l) : "r"(line));

        *kutil::assert::apic_icr = kutil::assert::send_nmi_command;

        while (1) asm ("hlt");
    }
}
