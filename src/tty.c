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

/**
 * Sets the active TTY to the selected TTY number
 * @param tty - TTY number
 */
void tty_select(int n) {
    if (n < 0 || n >= TTY_MAX) {
        kernel_panic("Invalid TTY %d", n);
    }

    active_tty = &tty_table[n];
    kernel_log_info("tty[%d]: selected", n);

    active_tty->refresh = 1;
}

/**
 * Returns the active TTY identifier
 * @return TTY id or -1 on error
 */
int tty_get_active(void) {
    if (!active_tty) {
        return -1;
    }

    return active_tty->id;
}

/**
 * Refreshes the tty if needed
 */
void tty_refresh(void) {

    //unsigned int buf_pos = active_tty->pos_x*(active_tty->pos_y + TTY_SCROLLBACK);
    
    if (!active_tty) {
        kernel_panic("No TTY is selected!");
        return;
    }


    if (active_tty->refresh) {
        kernel_log_debug("tty[%d]: refreshing", active_tty->id);

        int x = 0;
        int y = 0;

        for (int i = 0; i < TTY_WIDTH * TTY_HEIGHT; i++) {
            if (x >= VGA_WIDTH) {
                x = 0;
                y++;
            }

            vga_putc_at(x++, y, active_tty->color_bg, active_tty->color_fg, active_tty->buf[active_tty->pos_scroll*TTY_WIDTH + i]);
        }

        // The screen has been refreshed, so clea the refresh flag
        active_tty->refresh = 0;
    }
}

/**
 * Updates the active TTY with the given character
 */
void tty_update(char c) {
    if (!active_tty) {
        return;
    }

    struct tty_t *tty = active_tty;

    printf("\nCurrent char pressed was: %d", c);
    //    kernel_log_debug("tty[%d]: input char=%c", tty->id, c);
    //    kernel_log_debug("  before scroll=%d, x=%d, y=%d", tty->pos_scroll, tty->pos_x, tty->pos_y);

    switch (c) {
        case '\t':
            tty->pos_x += 4 - tty->pos_x % 4;
            break;

        case '\b':
            if (tty->pos_x != 0) {
                tty->pos_x--;
            } else if (tty->pos_y != 0) {
                tty->pos_y--;
                tty->pos_x = TTY_WIDTH - 1;
            }
            break;

        case '\r':
            tty->pos_x = 0;
            break;

        case '\n':
            tty->pos_y++;
            tty->pos_x = 0;
            break;

        default:
            tty->buf[(tty->pos_scroll * TTY_WIDTH) + (tty->pos_x + tty->pos_y * TTY_WIDTH)] = c;
            tty->pos_x++;
            break;
    }

    if (tty->pos_y >= TTY_HEIGHT) {
        int x;
        int y;

        for (x = 0; x < TTY_WIDTH; x++) {
            for (y = 1; y < TTY_HEIGHT; y++) {
                tty->buf[TTY_WIDTH * (y - 1) + x] = tty->buf[TTY_WIDTH * y + x];
            }
        }

        for (x = 0; x < TTY_WIDTH; x++) {
            tty->buf[TTY_WIDTH * (y - 1) + x] = ' ';
        }

        tty->pos_y = TTY_HEIGHT - 1;
    }

//    kernel_log_debug("  after: scroll=%d, x=%d, y=%d", tty->pos_scroll, tty->pos_x, tty->pos_y);
    tty->refresh = 1;
}

/**
 * Initializes all TTY data structures and memory
 * Selects TTY 0 to be the default
 */

void tty_init(void) {
    kernel_log_info("tty: Initializing TTY driver");

    // Initialize the tty_table
    memset(tty_table, 0, sizeof(tty_table));

    for(int i = 0; i< TTY_MAX; i++){
        tty_table[i].id=i;
        tty_table[i].color_bg = VGA_COLOR_BLACK;
        tty_table[i].color_fg = VGA_COLOR_LIGHT_GREY;
    }
    

    // Select tty 0 to start with
    tty_select(0);

    // Register a timer callback to update the screen on a regular interval  (50 times per second right now)
    timer_callback_register(&tty_refresh, 2, -1);
}

