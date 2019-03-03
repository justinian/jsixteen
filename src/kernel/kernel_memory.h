#pragma once
/// \file kernel_memory.h
/// Constants related to the kernel's memory layout

namespace memory {

	/// Size of a single page frame.
	static const size_t frame_size = 0x1000;

	/// Start of kernel memory.
	static const uintptr_t kernel_offset = 0xffffff0000000000;

	/// Offset from physical where page tables are mapped.
	static const uintptr_t page_offset = 0xffffff8000000000;

	/// Initial process thread's stack address
	static const uintptr_t initial_stack = 0x0000800000000000;

	/// Initial process thread's stack size, in pages
	static const unsigned initial_stack_pages = 1;

} // namespace memory
