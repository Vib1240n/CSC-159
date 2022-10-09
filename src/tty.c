/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * Fall 2022
 *
 * TTY Definitions
 */

#include <spede/string.h>

#include "kernel.h"
#include "timer.h"
#include "tty.h"
#include "vga.h"

// TTY Table
struct tty_t tty_table[TTY_MAX];

// Current Active TTY
struct tty_t *active_tty;

void tty_refersh(void);

/**
 * Sets the active TTY to the selected TTY number
 * @param tty - TTY number
 */
void tty_select(int n) {
    // Set the active tty to point to the entry in the tty table
    active_tty = &tty_table[n];
    // if a new tty is selected, the tty should trigger a refresh
    if(active_tty->refresh != 0){
        // tty_refresh();
    }
}

/**
 * Refreshes the tty if needed
 */
void tty_refresh(void) {

    unsigned int buf_pos = active_tty->pos_x*(active_tty->pos_y + TTY_SCROLLBACK);
    
    if (!active_tty) {
        kernel_panic("No TTY is selected!");
        return;
    }


    if(active_tty->refresh == 1) {


        vga_putc_at(active_tty->pos_x, active_tty->pos_y, active_tty->color_bg, active_tty->color_fg, active_tty->buf[buf_pos]);


        active_tty->refresh = 0;
    }

    // If the TTY needs to be refreshed, copy the tty buffer
    // to the VGA output.
    // ** hint: use vga_putc_at() since you are setting specific characters
    //          at specific locations
    // Reset the tty's refresh flag so we don't refresh unnecessarily
}

/**
 * Updates the active TTY with the given character
 */
void tty_update(char c) {
    if (!active_tty) {
        return;
    }
    unsigned int buf_pos = active_tty->pos_x*(active_tty->pos_y + TTY_SCROLLBACK);
    // Since this is a virtual wrapper around the VGA display, treat each
    // input character as you would for the VGA output
    //   Adjust the x/y positions as necessary
    //   Handle scrolling at the bottom

    

    active_tty->buf[buf_pos] = (unsigned short)VGA_CHAR(active_tty->color_bg, active_tty->color_fg, c);
    active_tty->pos_x += 1;


    if(active_tty->pos_x > TTY_WIDTH -1){
        active_tty->pos_x = 0;
        active_tty->pos_y += 1;
    }

    active_tty->refresh = 1;
    tty_refresh();



    // Instead of writing to the VGA display directly, you will write
    // to the tty buffer.
    //
    // If the screen should be updated, the refresh flag should be set
    // to trigger the the VGA update via the tty_refresh callback
}

/**
 * Initializes all TTY data structures and memory
 * Selects TTY 0 to be the default
 */

void tty_init(void) {
    kernel_log_info("tty: Initializing TTY driver");

    // Initialize the tty_table
    for(int i = 0; i< TTY_MAX; i++){
        tty_table[i].id = 0;
        tty_table[i].pos_x = 0;
        tty_table[i].pos_y = 0;
        tty_table[i].pos_scroll = 0;
        tty_table[i].color_bg = VGA_COLOR_BLACK;
        tty_table[i].color_fg = VGA_COLOR_WHITE;
        tty_table[i].refresh = 0;
        tty_table[i].buf[i] = 0;
    }
    

    // Select tty 0 to start with
    tty_select(0);

    // Register a timer callback to update the screen on a regular interval
    timer_callback_register(&tty_refresh, 100, -1);
}

