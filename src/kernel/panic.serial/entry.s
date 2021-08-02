%include "push_all.inc"

section .text

extern panic_handler

global _panic_entry
_panic_entry:
	cli
	push 0 ; NMI doesn't push an error code
	push 2 ; NMI is int 2
	push_all
	mov rdx, rsp

	mov rax, [rsp + REGS.rip]
	push rax

	jmp panic_handler