#pragma once
/// \file memory_pages.h
/// The page memory manager and related definitions.

#include <stdint.h>

#include "kutil/enum_bitfields.h"

struct page_block;
struct free_page;


/// Manager for allocation of physical pages.
class page_manager
{
public:
	page_manager();

	page_manager(const page_manager &) = delete;

private:
	friend void memory_initialize_managers(const void *, size_t, size_t);

	/// Set up the memory manager from bootstraped memory
	static void init(
			page_block *free,
			page_block *used,
			page_block *block_cache,
			uint64_t scratch_start,
			uint64_t scratch_length,
			uint64_t scratch_cur);

	/// Initialize the virtual memory manager based on this object's state
	void init_memory_manager();

	page_block *m_free; ///< Free pages list
	page_block *m_used; ///< In-use pages list

	page_block *m_block_cache; ///< Cache of unused page_block structs
	free_page *m_page_cache; ///< Cache of free pages to use for tables
};

/// Global page manager.
extern page_manager g_page_manager;


/// Flags used by `page_block`.
enum class page_block_flags : uint32_t
{
	free         = 0x00000000,  ///< Not a flag, value for free memory
	used         = 0x00000001,  ///< Memory is in use
	mapped       = 0x00000002,  ///< Memory is mapped to virtual address
	pending_free = 0x00000004,  ///< Memory should be freed

	nonvolatile  = 0x00000010,  ///< Memory is non-volatile storage
	acpi_wait    = 0x00000020,  ///< Memory should be freed after ACPI init
	permanent    = 0x80000000,  ///< Memory is permanently unusable

	max_flags
};
IS_BITFIELD(page_block_flags);


/// A block of contiguous pages. Each `page_block` represents contiguous
/// physical pages with the same attributes. A `page_block *` is also a
/// linked list of such structures.
struct page_block
{
	uint64_t physical_address;
	uint64_t virtual_address;
	uint32_t count;
	page_block_flags flags;
	page_block *next;

	bool has_flag(page_block_flags f) const { return bitfield_contains(flags, f); }
	uint64_t physical_end() const { return physical_address + (count * 0x1000); }
	uint64_t virtual_end() const { return virtual_address + (count * 0x1000); }

	/// Traverse the list, joining adjacent blocks where possible.
	/// \returns  A linked list of freed page_block structures.
	page_block * list_consolidate();

	/// Traverse the list, printing debug info on this list.
	/// \arg name  [optional] String to print as the name of this list
	void list_dump(const char *name = nullptr);
};


/// Helper struct for computing page table indices of a given address.
struct page_table_indices
{
	page_table_indices(uint64_t v = 0) :
		index{
			(v >> 39) & 0x1ff,
			(v >> 30) & 0x1ff,
			(v >> 21) & 0x1ff,
			(v >> 12) & 0x1ff }
	{}

	/// Get the index for a given level of page table.
	uint64_t & operator[](size_t i) { return index[i]; }
	uint64_t index[4]; ///< Indices for each level of tables.
};


/// Calculate a page-aligned address.
/// \arg p    The address to align.
/// \returns  The next page-aligned address _after_ `p`.
template <typename T> inline T page_align(T p)       { return ((p - 1) & ~0xfffull) + 0x1000; }

/// Calculate a page-table-aligned address. That is, an address that is
/// page-aligned to the first page in a page table.
/// \arg p    The address to align.
/// \returns  The next page-table-aligned address _after_ `p`.
template <typename T> inline T page_table_align(T p) { return ((p - 1) & ~0x1fffffull) + 0x200000; }