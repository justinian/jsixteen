#pragma once
#ifndef _uefi_protos_loaded_image_h_
#define _uefi_protos_loaded_image_h_

// This file was auto generated by the j6-uefi-headers project. Please see
// https://github.com/justinian/j6-uefi-headers for more information.

#include <uefi/guid.h>
#include <uefi/types.h>
#include <uefi/tables.h>
#include <uefi/protos/device_path.h>

namespace uefi {
namespace protos {
struct loaded_image;

struct loaded_image
{
    static constexpr uefi::guid guid{ 0x5b1b31a1,0x9562,0x11d2,{0x8e,0x3f,0x00,0xa0,0xc9,0x69,0x72,0x3b} };


    inline uefi::status unload(uefi::handle image_handle) {
        return _unload(image_handle);
    }

    uint32_t revision;
    uefi::handle parent_handle;
    uefi::system_table * system_table;
    uefi::handle device_handle;
    uefi::protos::device_path * file_path;
    void * reserved;
    uint32_t load_options_size;
    void * load_options;
    void * image_base;
    uint64_t image_size;
    uefi::memory_type image_code_type;
    uefi::memory_type image_data_type;

protected:
    using _unload_def = uefi::status (*)(uefi::handle);
    _unload_def _unload;

public:
};

} // namespace protos
} // namespace uefi

#endif // _uefi_protos_loaded_image_h_