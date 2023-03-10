#pragma once

#include <stdint.h>
#include <util/counted.h>

namespace panicking {

class symbol_table
{
public:
    /// Constructor.
    /// \arg data  Pointer to the start of the symbol_table data.
    symbol_table(const void *data);

    /// Find the name of the symbol at the given address
    /// \args addr  Address to search for
    /// \returns    Name of the symbol if found, or null
    const char * find_symbol(uintptr_t addr) const;

private:
    struct entry
    {
        uintptr_t address;
        size_t size;
        uintptr_t name;
    };

    const void *m_data;
    util::counted<entry const> m_entries;
};

} // namespace panicking
