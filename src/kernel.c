/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * Fall 2022
 *
 * Kernel functions
 */
#include <spede/flames.h>
#include <spede/stdarg.h>
#include <spede/stdio.h>
#include <spede/string.h>

#include "kernel.h"

#ifndef KERNEL_LOG_LEVEL_DEFAULT
#define KERNEL_LOG_LEVEL_DEFAULT KERNEL_LOG_LEVEL_INFO
#endif

// Current log level
int kernel_log_level = KERNEL_LOG_LEVEL_DEFAULT;

/**
 * Initializes any kernel internal data structures and variables
 */
void kernel_init() {
    // Display a welcome message on the host
    kernel_log_info("Welcome to %s!", OS_NAME);

    kernel_log_info("Initializing kernel...");
}

/**
 * Prints a kernel log message to the host with an error log level
 *
 * @param msg - string format for the message to be displayed
 * @param ... - variable arguments to pass in to the string format
 */
void kernel_log_error(char *msg, ...) {
}

/**
 * Prints a kernel log message to the host with a warning log level
 *
 * @param msg - string format for the message to be displayed
 * @param ... - variable arguments to pass in to the string format
 */
void kernel_log_warn(char *msg, ...) {
}

/**
 * Prints a kernel log message to the host with an info log level
 *
 * @param msg - string format for the message to be displayed
 * @param ... - variable arguments to pass in to the string format
 */
void kernel_log_info(char *msg, ...) {
    // Return if our log level is less than info
    if (kernel_log_level < KERNEL_LOG_LEVEL_INFO) {
        return;
    }

    // Obtain the list of variable arguments
    va_list args;

    // Indicate this is an 'info' type of message
    printf("info: ");

    // Pass the message variable arguments to vprintf
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);

    printf("\n");
}

/**
 * Prints a kernel log message to the host with a debug log level
 *
 * @param msg - string format for the message to be displayed
 * @param ... - variable arguments to pass in to the string format
 */
void kernel_log_debug(char *msg, ...) {
}

/**
 * Prints a kernel log message to the host with a trace log level
 *
 * @param msg - string format for the message to be displayed
 * @param ... - variable arguments to pass in to the string format
 */
void kernel_log_trace(char *msg, ...) {
}

/**
 * Triggers a kernel panic that does the following:
 *   - Displays a panic message on the host console
 *   - Triggers a breakpiont (if running through GDB)
 *   - aborts/exits the operating system program
 *
 * @param msg - string format for the message to be displayed
 * @param ... - variable arguments to pass in to the string format
 */
void kernel_panic(char *msg, ...) {
    va_list args;

    printf("panic: ");

    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);

    printf("\n");

    breakpoint();
    exit(1);
}

