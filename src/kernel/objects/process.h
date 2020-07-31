#pragma once
/// \file process.h
/// Definition of process kobject types

#include "objects/kobject.h"
#include "page_table.h"

class process :
	public kobject
{
public:
	/// Top of memory area where thread stacks are allocated
	constexpr static uintptr_t stacks_top = 0x0000800000000000;

	/// Size of userspace thread stacks
	constexpr static size_t stack_size = 0x4000;

	/// Constructor.
	/// \args pml4  Root of the process' page tables
	process(page_table *pml4);

	/// Destructor.
	virtual ~process();

	/// Terminate this process.
	/// \arg code   The return code to exit with.
	void exit(unsigned code);

	/// Update internal bookkeeping about threads.
	void update();

	/// Get the process' page table root
	page_table * pml4() { return m_pml4; }

	/// Create a new thread in this process
	/// \args priority  The new thread's scheduling priority
	/// \args user      If true, create a userspace stack for this thread
	/// \returns        The newly created thread object
	thread * create_thread(uint8_t priorty, bool user = true);

	/// Start tracking an object with a handle.
	/// \args obj  The object this handle refers to
	/// \returns   The new handle for this object
	j6_handle_t add_handle(kobject *obj);

	/// Stop tracking an object with a handle.
	/// \args handle The handle that refers to the object
	/// \returns     True if the handle was removed
	bool remove_handle(j6_handle_t handle);

	/// Lookup an object for a handle
	/// \args handle The handle to the object
	/// \returns     Pointer to the object, or null if not found
	kobject * lookup_handle(j6_handle_t handle);

	/// Inform the process of an exited thread
	/// \args th  The thread which has exited
	/// \returns  True if this thread ending has ended the process
	bool thread_exited(thread *th);

	/// Create the special kernel process that owns kernel tasks
	/// \arg pml4     The kernel-only pml4
	/// \arg idle_rsp The idle thread's rsp
	static process * create_kernel_process(page_table *pml4, uintptr_t idle_rsp);

private:
	uint32_t m_return_code;

	page_table *m_pml4;
	kutil::vector<thread*> m_threads;
	kutil::vector<kobject*> m_handles;

	enum class state : uint8_t { running, exited };
	state m_state;

	static kutil::vector<process*> s_processes;
};