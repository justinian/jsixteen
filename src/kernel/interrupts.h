#pragma once
/// \file interrupts.h
/// Free functions and definitions related to interrupt service vectors
#include <stdint.h>


/// Enum of all defined ISR/IRQ vectors
enum class isr : uint8_t
{
#define ISR(i, s, name)     name = i,
#define NISR(i, s, name)    name = i,
#define EISR(i, s, name)    name = i,
#define IRQ(i, q, name)     name = i,
#include "interrupt_isrs.inc"
#undef IRQ
#undef EISR
#undef NISR
#undef ISR

    _zero = 0
};

/// Helper operator to add an offset to an isr vector
constexpr isr operator+(const isr &lhs, int rhs) {
    return static_cast<isr>(static_cast<uint8_t>(lhs) + rhs);
}

extern "C" {
    /// Set the CPU interrupt enable flag (sti)
    void interrupts_enable();

    /// Set the CPU interrupt disable flag (cli)
    void interrupts_disable();
}

/// Disable the legacy PIC
void disable_legacy_pic();
