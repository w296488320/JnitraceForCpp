/* Copyright (C) 1997-2018 Free Software Foundation, Inc.

   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _AARCH64_SYSDEP_H
#define _AARCH64_SYSDEP_H

#ifndef C_LABEL

/* Define a macro we can use to construct the asm name for a C symbol.  */
# define C_LABEL(name)  name##:

#endif

#ifdef __ASSEMBLER__
/* Mark the end of function named SYM.  This is used on some platforms
   to generate correct debugging information.  */
# ifndef END
#  define END(sym)
# endif

# ifndef JUMPTARGET
#  define JUMPTARGET(sym) sym
# endif
#endif

/* Makros to generate eh_frame unwind information.  */
#ifdef __ASSEMBLER__
# define cfi_startproc      .cfi_startproc
# define cfi_endproc      .cfi_endproc
# define cfi_def_cfa(reg, off)    .cfi_def_cfa reg, off
# define cfi_def_cfa_register(reg)  .cfi_def_cfa_register reg
# define cfi_def_cfa_offset(off)  .cfi_def_cfa_offset off
# define cfi_adjust_cfa_offset(off) .cfi_adjust_cfa_offset off
# define cfi_offset(reg, off)   .cfi_offset reg, off
# define cfi_rel_offset(reg, off) .cfi_rel_offset reg, off
# define cfi_register(r1, r2)   .cfi_register r1, r2
# define cfi_return_column(reg) .cfi_return_column reg
# define cfi_restore(reg)   .cfi_restore reg
# define cfi_same_value(reg)    .cfi_same_value reg
# define cfi_undefined(reg)   .cfi_undefined reg
# define cfi_remember_state   .cfi_remember_state
# define cfi_restore_state    .cfi_restore_state
# define cfi_window_save    .cfi_window_save
# define cfi_personality(enc, exp)  .cfi_personality enc, exp
# define cfi_lsda(enc, exp)   .cfi_lsda enc, exp

#else /* ! ASSEMBLER */

# define CFI_STRINGIFY(Name) CFI_STRINGIFY2 (Name)
# define CFI_STRINGIFY2(Name) #Name
# define CFI_STARTPROC  ".cfi_startproc"
# define CFI_ENDPROC  ".cfi_endproc"
# define CFI_DEF_CFA(reg, off)  \
   ".cfi_def_cfa " CFI_STRINGIFY(reg) "," CFI_STRINGIFY(off)
# define CFI_DEF_CFA_REGISTER(reg) \
   ".cfi_def_cfa_register " CFI_STRINGIFY(reg)
# define CFI_DEF_CFA_OFFSET(off) \
   ".cfi_def_cfa_offset " CFI_STRINGIFY(off)
# define CFI_ADJUST_CFA_OFFSET(off) \
   ".cfi_adjust_cfa_offset " CFI_STRINGIFY(off)
# define CFI_OFFSET(reg, off) \
   ".cfi_offset " CFI_STRINGIFY(reg) "," CFI_STRINGIFY(off)
# define CFI_REL_OFFSET(reg, off) \
   ".cfi_rel_offset " CFI_STRINGIFY(reg) "," CFI_STRINGIFY(off)
# define CFI_REGISTER(r1, r2) \
   ".cfi_register " CFI_STRINGIFY(r1) "," CFI_STRINGIFY(r2)
# define CFI_RETURN_COLUMN(reg) \
   ".cfi_return_column " CFI_STRINGIFY(reg)
# define CFI_RESTORE(reg) \
   ".cfi_restore " CFI_STRINGIFY(reg)
# define CFI_UNDEFINED(reg) \
   ".cfi_undefined " CFI_STRINGIFY(reg)
# define CFI_REMEMBER_STATE \
   ".cfi_remember_state"
# define CFI_RESTORE_STATE \
   ".cfi_restore_state"
# define CFI_WINDOW_SAVE \
   ".cfi_window_save"
# define CFI_PERSONALITY(enc, exp) \
   ".cfi_personality " CFI_STRINGIFY(enc) "," CFI_STRINGIFY(exp)
# define CFI_LSDA(enc, exp) \
   ".cfi_lsda " CFI_STRINGIFY(enc) "," CFI_STRINGIFY(exp)
#endif



#define HAVE_SYSCALLS

/* Note that using a `PASTE' macro loses.  */
#define SYSCALL__(name, args) PSEUDO (__##name, name, args)
#define SYSCALL(name, args) PSEUDO (name, name, args)

/* Machine-dependent sysdep.h files are expected to define the macro
   PSEUDO (function_name, syscall_name) to emit assembly code to define the
   C-callable function FUNCTION_NAME to do system call SYSCALL_NAME.
   r0 and r1 are the system call outputs.  MOVE(x, y) should be defined as
   an instruction such that "MOVE(r1, r0)" works.  ret should be defined
   as the return instruction.  */

#define SYS_ify(syscall_name) SYS_##syscall_name

/* Terminate a system call named SYM.  This is used on some platforms
   to generate correct debugging information.  */
#ifndef PSEUDO_END
#define PSEUDO_END(sym)
#endif
#ifndef PSEUDO_END_NOERRNO
#define PSEUDO_END_NOERRNO(sym) PSEUDO_END(sym)
#endif
#ifndef PSEUDO_END_ERRVAL
#define PSEUDO_END_ERRVAL(sym)  PSEUDO_END(sym)
#endif

/* Wrappers around system calls should normally inline the system call code.
   But sometimes it is not possible or implemented and we use this code.  */
#define INLINE_SYSCALL(name, nr, args...) __syscall_##name (args)




#ifdef  __ASSEMBLER__

/* Syntactic details of assembler.  */

#define ASM_SIZE_DIRECTIVE(name) .size name,.-name

/* Define an entry point visible from C.  */
#define ENTRY(name)           \
  .globl C_SYMBOL_NAME(name);         \
  .type C_SYMBOL_NAME(name),%function;        \
  .align 4;             \
  C_LABEL(name)             \
  cfi_startproc;            \
  CALL_MCOUNT

/* Define an entry point visible from C.  */
#define ENTRY_ALIGN(name, align)        \
  .globl C_SYMBOL_NAME(name);         \
  .type C_SYMBOL_NAME(name),%function;        \
  .p2align align;           \
  C_LABEL(name)             \
  cfi_startproc;            \
  CALL_MCOUNT

/* Define an entry point visible from C with a specified alignment and
   pre-padding with NOPs.  This can be used to ensure that a critical
   loop within a function is cache line aligned.  Note this version
   does not adjust the padding if CALL_MCOUNT is defined. */

#define ENTRY_ALIGN_AND_PAD(name, align, padding)   \
  .globl C_SYMBOL_NAME(name);         \
  .type C_SYMBOL_NAME(name),%function;        \
  .p2align align;           \
  .rep padding;             \
  nop;                \
  .endr;              \
  C_LABEL(name)             \
  cfi_startproc;            \
  CALL_MCOUNT

#undef  END
#define END(name)           \
  cfi_endproc;              \
  ASM_SIZE_DIRECTIVE(name)

/* If compiled for profiling, call `mcount' at the start of each function.  */
#ifdef  PROF
# define CALL_MCOUNT            \
  str x30, [sp, #-16]!;       \
  bl  mcount;           \
  ldr x30, [sp], #16  ;
#else
# define CALL_MCOUNT    /* Do nothing.  */
#endif

/* Local label name for asm code.  */
#ifndef L
# define L(name)         .L##name
#endif

/* Load or store to/from a pc-relative EXPR into/from R, using T.  */
#define LDST_PCREL(OP, R, T, EXPR)  \
  adrp  T, EXPR;      \
  OP  R, [T, #:lo12:EXPR];\

/* Load or store to/from a got-relative EXPR into/from R, using T.  */
#define LDST_GLOBAL(OP, R, T, EXPR)     \
  adrp  T, :got:EXPR;   \
  ldr T, [T, #:got_lo12:EXPR];\
  OP  R, [T];

/* Since C identifiers are not normally prefixed with an underscore
   on this system, the asm identifier `syscall_error' intrudes on the
   C name space.  Make sure we use an innocuous name.  */
#define syscall_error __syscall_error
#define mcount    _mcount

#endif  /* __ASSEMBLER__ */


/* Provide the common name to allow more code reuse.  */
#define __NR__llseek __NR_llseek

#if __WORDSIZE == 64
/* By defining the older names, glibc will build syscall wrappers for
   both pread and pread64; sysdeps/unix/sysv/linux/wordsize-64/pread64.c
   will suppress generating any separate code for pread64.c.  */
#define __NR_pread __NR_pread64
#define __NR_pwrite __NR_pwrite64
#endif

/* Provide a dummy argument that can be used to force register
   alignment for register pairs if required by the syscall ABI.  */
#ifdef __ASSUME_ALIGNED_REGISTER_PAIRS
#define __ALIGNMENT_ARG 0,
#define __ALIGNMENT_COUNT(a,b) b
#else
#define __ALIGNMENT_ARG
#define __ALIGNMENT_COUNT(a,b) a
#endif


#ifndef C_SYMBOL_NAME
# define C_SYMBOL_NAME(name) name
#endif

#endif  /* _AARCH64_SYSDEP_H */
