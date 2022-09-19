#include <spede/stdio.h>
#include <spede/machine/io.h>

#include "kernel.h"
#include "keyboard.h"

// Keyboard data port
#define KBD_PORT_DATA           0x60

// Keyboard status port
#define KBD_PORT_STAT           0x64

// Keyboard scancode definitions
#define KEY_CTRL_L              0x1D
#define KEY_CTRL_R              0xE01D

#define KEY_ALT_L               0x38
#define KEY_ALT_R               0xE038

#define KEY_SHIFT_L             0x2A
#define KEY_SHIFT_R             0x36

#define KEY_CAPS                0x3A
#define KEY_NUMLOCK             0x45


/**
 * Initializes keyboard data structures and variables
 */
void keyboard_init() {
    kernel_log_info("Initializing keyboard");
}

/**
 * Scans for keyboard input and returns the raw character data
 * @return raw character data from the keyboard
 */
unsigned int keyboard_scan(void) {
    unsigned int c = KEY_NULL;
    return c;
}

/**
 * Polls for a keyboard character to be entered.
 *
 * If a keyboard character data is present, will scan and return
 * the decoded keyboard output.
 *
 * @return decoded character or KEY_NULL (0) for any character
 *         that cannot be decoded
 */
unsigned int keyboard_poll(void) {
    unsigned int c = KEY_NULL;
    return c;
}

/**
 * Blocks until a keyboard character has been entered
 * @return decoded character entered by the keyboard or KEY_NULL
 *         for any character that cannot be decoded
 */
unsigned int keyboard_getc(void) {
    unsigned int c = KEY_NULL;
    while ((c = keyboard_poll()) == KEY_NULL);
    return c;
}

/**
 * Processes raw keyboard input and decodes it.
 *
 * Should keep track of the keyboard status for the following keys:
 *   SHIFT, CTRL, ALT, CAPS, NUMLOCK
 *
 * For all other characters, they should be decoded/mapped to ASCII
 * or ASCII-friendly characters.
 *
 * For any character that cannot be mapped, KEY_NULL should be returned.
 */
unsigned int keyboard_decode(unsigned int c) {
    return KEY_NULL;
}
