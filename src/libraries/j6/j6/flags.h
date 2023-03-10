#pragma once
/// \file flags.h
/// Enums used as flags for syscalls

enum j6_vm_flags {
#define VM_FLAG(name, v) j6_vm_flag_ ## name = v,
#include <j6/tables/vm_flags.inc>
#undef VM_FLAG
    j6_vm_flag_MAX
};

enum j6_channel_flags {
    j6_channel_block = 0x01,
};

enum j6_mailbox_flags {
    j6_mailbox_block = 0x01,
};
