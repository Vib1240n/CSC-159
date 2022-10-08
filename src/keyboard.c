#include <spede/stdio.h>
#include <spede/machine/io.h>

#include "kernel.h"
#include "keyboard.h"
#include "interrupts.h"
#include "vga.h"

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

#define RELEASED                0x80

int ctrl;
int alt;
int shift;
int caps;
int numlock;

int esc_count = 0;

void keyboard_irq_handler(void) {
    unsigned int c = keyboard_poll();
    if (c) {
        vga_putc(c);
    }
}

/**
 * Initializes keyboard data structures and variables
 */
void keyboard_init() {
    kernel_log_info("Initializing keyboard");
    ctrl = 0;
    alt = 0;
    shift = 0;
    caps = 0;
    numlock = 0;

    interrupts_irq_register(IRQ_KEYBOARD, isr_entry_keyboard, keyboard_irq_handler);
}

/**
 * Scans for keyboard input and returns the raw character data
 * @return raw character data from the keyboard
 */
unsigned int keyboard_scan(void) {
    unsigned int c = inportb(KBD_PORT_DATA);
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
    //unsigned int status = inportb(KBD_PORT_STAT);
    if ((inportb(KBD_PORT_STAT) & 0x1) == 1) {
        c = keyboard_scan();
        c = keyboard_decode(c);
        //kernel_log_info("Key press [%c] - %u [0x%x]", c, d, d);

        //QUIT WITH ESC THREE TIMEs
        if (c == KEY_ESCAPE) {
            esc_count++;

            if (esc_count == 3) {
                kernel_exit();
            }

            c = KEY_NULL;
        } else if (c != KEY_NULL) {
            esc_count = 0;
        }

        //Ctrl+ increase log level
        if (c == 0x3D && ctrl == 1) {
            kernel_set_log_level(kernel_get_log_level() + 1);
            return KEY_NULL;
        }

        //Ctrl- decrease log level
        if (c == 0x2D && ctrl == 1) {
            kernel_set_log_level(kernel_get_log_level() - 1);
            return KEY_NULL;
        }
    }
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
    switch (c) {

        case KEY_CTRL_L:                //Left Ctrl PR
            ctrl = !ctrl;
            break;
            //return KEY_CTRL_L;
        case KEY_CTRL_L | RELEASED:     //Left Ctrl RL
            ctrl = !ctrl;
            break;
            //return KEY_CTRL_L | RELEASED;
        case KEY_CTRL_R:                //Right Ctrl PR
            ctrl = !ctrl;
            break;
            //return KEY_CTRL_R;
        case KEY_CTRL_R | RELEASED:     //Right Ctrl RL
            ctrl = !ctrl;
            break;
            //return KEY_CTRL_R | RELEASED;

        case KEY_ALT_L:                 //Left ALT PR
            alt = !alt;
            break;
            //return KEY_ALT_L;
        case KEY_ALT_L | RELEASED:      //Left ALT RL
            alt = !alt;
            break;
            //return KEY_ALT_L | RELEASED;
        case KEY_ALT_R:                 //Right ALT PR
            alt = !alt;
            break;
            //return KEY_ALT_R;
        case KEY_ALT_R | RELEASED:      //Right ALT RL
            alt = !alt;
            break;
            //return KEY_ALT_R | RELEASED;

        case KEY_SHIFT_L:               //Left Shift PR
            shift = !shift;
            break;
            //return KEY_SHIFT_L;
        case KEY_SHIFT_L | RELEASED:    //Left Shift RL
            shift = !shift;
            break;
            //return KEY_SHIFT_L | RELEASED;
        case KEY_SHIFT_R:               //Right Shift PR
            shift = !shift;
            break;
            //return KEY_SHIFT_R;
        case KEY_SHIFT_R | RELEASED:    //Right Shift RL
            shift = !shift;
            break;
            //return KEY_SHIFT_R | RELEASED;

        case KEY_CAPS:                  //Caps Lock
            caps = !caps;
            break;
            //return KEY_CAPS;
        case KEY_NUMLOCK:               //Num Lock
            numlock = !numlock;
            break;
            //return KEY_NUMLOCK;

        case 0x01:              //Escape
            return KEY_ESCAPE;
        case 0x02:              //1!
            if (shift || caps)
                return 0x21;
            else
                return 0x31;
        case 0x03:              //2@
            if (shift || caps)
                return 0x40;
            else
                return 0x32;
        case 0x04:              //3#
            if (shift || caps)
                return 0x23;
            else
                return 0x33;
        case 0x05:              //4$
            if (shift || caps)
                return 0x24;
            else
                return 0x34;
        case 0x06:              //5%
            if (shift || caps)
                return 0x25;
            else
                return 0x35;
        case 0x07:              //6^
            if (shift || caps)
                return 0x5E;
            else
                return 0x36;
        case 0x08:              //7&
            if (shift || caps)
                return 0x26;
            else
                return 0x37;
        case 0x09:              //8*
            if (shift || caps)
                return 0x2A;
            else
                return 0x38;
        case 0x0a:              //9(
            if (shift || caps)
                return 0x28;
            else
                return 0x39;
        case 0x0b:              //0)
            if (shift || caps)
                return 0x29;
            else
                return 0x30;
        case 0x0c:              //-_
            if (shift || caps)
                return 0x5F;
            else
                return 0x2D;
        case 0x0d:              //=+
            if (shift || caps)
                return 0x2B;
            else
                return 0x3D;
        case 0x0e:              //Backspace
            return 0x08;
        case 0x0f:              //Tab
            return 0x09;
        case 0x10:              //Qq
            if (shift || caps)
                return 0x51;
            else
                return 0x71;
        case 0x11:              //Ww
            if (shift || caps)
                return 0x57;
            else
                return 0x77;
        case 0x12:              //Ee
            if (shift || caps)
                return 0x45;
            else
                return 0x65;
        case 0x13:              //Rr
            if (shift || caps)
                return 0x52;
            else
                return 0x72;
        case 0x14:              //Tt
            if (shift || caps)
                return 0x54;
            else
                return 0x74;
        case 0x15:              //Yy
            if (shift || caps)
                return 0x59;
            else
                return 0x79;
        case 0x16:              //Uu
            if (shift || caps)
                return 0x55;
            else
                return 0x75;
        case 0x17:              //Ii
            if (shift || caps)
                return 0x49;
            else
                return 0x69;
        case 0x18:              //Oo
            if (shift || caps)
                return 0x4F;
            else
                return 0x6F;
        case 0x19:              //Pp
            if (shift || caps)
                return 0x50;
            else
                return 0x70;
        case 0x1a:              //[{
            if (shift || caps)
                return 0x7B;
            else
                return 0x5B;
        case 0x1b:              //]}
            if (shift || caps)
                return 0x7D;
            else
                return 0x5D;
        case 0x1c:              //Enter
           return 0xA;
        case 0x1e:              //Aa
            if (shift || caps)
                return 0x41;
            else
                return 0x61;
        case 0x1f:              //Ss
            if (shift || caps)
                return 0x53;
            else
                return 0x73;
        case 0x20:              //Dd
            if (shift || caps)
                return 0x44;
            else
                return 0x64;
        case 0x21:              //Ff
            if (shift || caps)
                return 0x46;
            else
                return 0x66;
        case 0x22:              //Gg
            if (shift || caps)
                return 0x47;
            else
                return 0x67;
        case 0x23:              //Hh
            if (shift || caps)
                return 0x48;
            else
                return 0x68;
        case 0x24:              //Jj
            if (shift || caps)
                return 0x4A;
            else
                return 0x6A;
        case 0x25:              //Kk
            if (shift || caps)
                return 0x4B;
            else
                return 0x6B;
        case 0x26:              //Ll
            if (shift || caps)
                return 0x4C;
            else
                return 0x6C;
        case 0x27:              //;:
            if (shift || caps)
                return 0x3A;
            else
                return 0x3B;
        case 0x28:              //'"
            if (shift || caps)
                return 0x22;
            else
                return 0x27;
        case 0x29:              //`~
            if (shift || caps)
                return 0x7E;
            else
                return 0x60;
        case 0x2b:              //\|
            if (shift || caps)
                return 0x7C;
            else
                return 0x5C;
        case 0x2c:              //Zz
            if (shift || caps)
                return 0x5A;
            else
                return 0x7A;
        case 0x2d:              //Xx
            if (shift || caps)
                return 0x58;
            else
                return 0x78;
        case 0x2e:              //Cc
            if (shift || caps)
                return 0x43;
            else
                return 0x63;
        case 0x2f:              //Vv
            if (shift || caps)
                return 0x56;
            else
                return 0x76;
        case 0x30:              //Bb
            if (shift || caps)
                return 0x42;
            else
                return 0x62;
        case 0x31:              //Nn
            if (shift || caps)
                return 0x4E;
            else
                return 0x6E;
        case 0x32:              //Mm
            if (shift || caps)
                return 0x4D;
            else
                return 0x6D;
        case 0x33:              //,<
            if (shift || caps)
                return 0x3C;
            else
                return 0x2C;
        case 0x34:              //.>
            if (shift || caps)
                return 0x3E;
            else
                return 0x2E;
        case 0x35:              ///?
            if (shift || caps)
                return 0x3F;
            else
                return 0x2F;
        case 0x39:              //Space
            return 0x20;
        case 0x3b:              //F1
            return KEY_F1;
        case 0x3c:              //F2
            return KEY_F2;
        case 0x3d:              //F3
            return KEY_F3;
        case 0x3e:              //F4
            return KEY_F4;
        case 0x3f:              //F5
            return KEY_F5;
        case 0x40:              //F6
            return KEY_F6;
        case 0x41:              //F7
            return KEY_F7;
        case 0x42:              //F8
            return KEY_F8;
        case 0x43:              //F9
            return KEY_F9;
        case 0x44:              //F10
            return KEY_F10;
        case 0x87:              //F11
            return KEY_F11;
        case 0x88:              //F12
            return KEY_F12;
        case 0x47:              //Home
            return KEY_HOME;
        case 0x48:              //Up
            return KEY_UP;
        case 0x49:              //Page Up
            return KEY_PAGE_UP;
        case 0x4b:              //Left
            return KEY_LEFT;
        case 0x4d:              //Right
            return KEY_RIGHT;
        case 0x4f:              //End
            return KEY_END;
        case 0x50:              //Down
            return KEY_DOWN;
        case 0x51:              //Page Down
            return KEY_PAGE_DOWN;
        case 0x52:              //Insert
            return KEY_INSERT;
        case 0x53:              //Delete
            return KEY_DELETE;

    }
    /**
    if (c == KEY_ESCAPE) {
        esc_count++;

        if (esc_count == 3) {
            kernel_exit();
        }

        return KEY_NULL;
    } else if (c != KEY_NULL) {
        esc_count = 0;
    }
    **/

    return KEY_NULL;
}
