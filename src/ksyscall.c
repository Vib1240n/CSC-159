/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * Fall 2022
 *
 * Kernel System Call Handlers
 */
#include <spede/time.h>
#include <spede/string.h>
#include <spede/stdio.h>

#include "kernel.h"
#include "kproc.h"
#include "ksyscall.h"
#include "interrupts.h"
#include "scheduler.h"
#include "timer.h"

/**
 * System call IRQ handler
 * Dispatches system calls to the function associate with the specified system call
 */
void ksyscall_irq_handler(void) {
    // Default return value
    int rc = -1;

    if (!active_proc) {
        kernel_panic("Invalid process");
    }

    if (!active_proc->trapframe) {
        kernel_panic("Invalid trapframe");
    }

    // System call identifier is stored on the EAX register
    // Additional arguments should be stored on additional registers (EBX, ECX, etc.)

    // Based upon the system call identifier, call the respective system call handler

    // Ensure that the EAX register for the active process contains the return value

    if (active_proc->trapframe->eax == SYSCALL_SYS_GET_TIME) {
        rc = ksyscall_sys_get_time();
        active_proc->trapframe->eax = rc;
        return;
    }

    if (active_proc->trapframe->eax == SYSCALL_SYS_GET_NAME) {
        // Cast the argument as a char pointer
        rc = ksyscall_sys_get_name((char *)active_proc->trapframe->ebx);
        active_proc->trapframe->eax = rc;
        return;
    }

    kernel_panic("Invalid system call %d!", active_proc->trapframe->eax);
}

/**
 * System Call Initialization
 */
void ksyscall_init(void) {
    // Register the IDT entry and IRQ handler for the syscall IRQ (IRQ_SYSCALL)
    interrupts_irq_register(IRQ_SYSCALL, isr_entry_syscall, ksyscall_irq_handler);
}

/**
 * Writes up to n bytes to the process' specified IO buffer
 * @param io - the IO buffer to write to
 * @param buf - the buffer to copy from
 * @param n - number of bytes to write
 * @return -1 on error or value indicating number of bytes copied
 */
int ksyscall_io_write(int io, char *buf, int size) {
    return 0;
}

/**
 * Reads up to n bytes from the process' specified IO buffer
 * @param io - the IO buffer to read from
 * @param buf - the buffer to copy to
 * @param n - number of bytes to read
 * @return -1 on error or value indicating number of bytes copied
 */
int ksyscall_io_read(int io, char *buf, int size) {
    return 0;
}

/**
 * Flushes (clears) the specified IO buffer
 * @param io - the IO buffer to flush
 * @return -1 on error or 0 on success
 */
int ksyscall_io_flush(int io) {
    return -1;
}

/**
 * Gets the current system time (in seconds)
 * @return system time in seconds
 */
int ksyscall_sys_get_time(void) {
    return timer_get_ticks() / 100;
}

/**
 * Gets the operating system name
 * @param name - pointer to a character buffer where the name will be copied
 * @return 0 on success, -1 or other non-zero value on error
 */
int ksyscall_sys_get_name(char *name) {
    if (!name) {
        return -1;
    }

    strncpy(name, OS_NAME, sizeof(OS_NAME));
    return 0;
}

/**
 * Puts the active process to sleep for the specified number of seconds
 * @param seconds - number of seconds the process should sleep
 */
int ksyscall_proc_sleep(int seconds) {
    return -1;
}

/**
 * Exits the current process
 */
int ksyscall_proc_exit(void) {
    return -1;
}

/**
 * Gets the active process pid
 * @return process id or -1 on error
 */
int ksyscall_proc_get_pid(void) {
    return -1;
}

/**
 * Gets the active process' name
 * @param name - pointer to a character buffer where the name will be copied
 * @return 0 on success, -1 or other non-zero value on error
 */
int ksyscall_proc_get_name(char *name) {
    return -1;
}