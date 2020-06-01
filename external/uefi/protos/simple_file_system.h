#pragma once
#ifndef _uefi_protos_simple_file_system_h_
#define _uefi_protos_simple_file_system_h_

// This file was auto generated by the j6-uefi-headers project. Please see
// https://github.com/justinian/j6-uefi-headers for more information.

#include <uefi/guid.h>
#include <uefi/types.h>
#include <uefi/protos/file.h>

namespace uefi {
namespace protos {
struct simple_file_system;

struct simple_file_system
{
    static constexpr uefi::guid guid{ 0x0964e5b22,0x6459,0x11d2,{0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b} };

    inline uefi::status open_volume(uefi::protos::file ** root) {
        return _open_volume(this, root);
    }

    uint64_t revision;

protected:
    using _open_volume_def = uefi::status (*)(uefi::protos::simple_file_system *, uefi::protos::file **);
    _open_volume_def _open_volume;

public:
};

} // namespace protos
} // namespace uefi

#endif // _uefi_protos_simple_file_system_h_