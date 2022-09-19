/**
 * CPE/CSC 159 Operating System Pragmatics
 * California State University, Sacramento
 * Fall 2022
 *
 * Operating system entry point
 */

#include "kernel.h"
#include "vga.h"
#include "keyboard.h"
#include <spede/stdbool.h>

int main(void) {
    unsigned char spinner[] = "/-\\|";
    int spin = 0;
    int counter = 0;
    int running = true;
    int esc_count = 0;
    unsigned char c;

    // Always iniialize the kernel
    kernel_init();

    // Initialize the VGA driver
    vga_init();

    // Initialize the keyboard driver
    keyboard_init();

    // Print a welcome message
    vga_printf("Welcome to %s!\n", OS_NAME);
    vga_puts("Press a key to continue...\n");

    // Wait for a key to be pressed
    keyboard_getc();

    // Clear the screen
    vga_clear();

    // Run loop
    while (running) {
        c = keyboard_poll();

        if (c == KEY_ESCAPE) {
            esc_count++;

            if (esc_count == 3) {
                running = false;
            }
            continue;
        } else if (c != KEY_NULL) {
            esc_count = 0;
        }

        switch (c) {
            case KEY_LEFT:
                vga_setc('.');
                vga_set_xy(vga_get_x() - 1, vga_get_y());
                break;

            case KEY_RIGHT:
                vga_setc('.');
                vga_set_xy(vga_get_x() + 1, vga_get_y());
                break;

            case KEY_UP:
                vga_setc('.');
                vga_set_xy(vga_get_x(), vga_get_y() - 1);
                break;

            case KEY_DOWN:
                vga_setc('.');
                vga_set_xy(vga_get_x(), vga_get_y() + 1);
                break;

            case KEY_NULL:
                break;

            default:
                vga_putc(c);
        }

        if ((counter++ % 876543) == 0) {
            vga_putc_at(VGA_WIDTH-1, 0,
                        VGA_COLOR_BLACK, VGA_COLOR_GREEN,
                        spinner[spin++ % (sizeof(spinner) - 1)]);
        }
    }

    vga_set_bg(VGA_COLOR_RED);
    vga_set_fg(VGA_COLOR_WHITE);
    vga_set_xy(0, 0);
    vga_printf("%*s", 80, "");
    vga_set_xy(0, 0);
    vga_printf("Exiting %s...\n", OS_NAME);

    return 0;
}
