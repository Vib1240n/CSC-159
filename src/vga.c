#include <spede/machine/io.h>
#include <spede/stdarg.h>
#include <spede/stdio.h>

#include "kernel.h"
#include "vga.h"

// VGA Address Port -> Set the register address to write data into
#define VGA_PORT_ADDR 0x3D4
// VGA Data Port -> The data to be written into the register
#define VGA_PORT_DATA 0x3D5

// Current x position (column)
int vga_pos_x = 0;

// Current y position (row)
int vga_pos_y = 0;

// Current background color, default to black
int vga_color_bg = VGA_COLOR_BLACK;

// Current foreground color, default to light grey
int vga_color_fg = VGA_COLOR_LIGHT_GREY;

// VGA text mode cursor status
int vga_cursor = 0;

/**
 * Initializes the VGA driver and configuration
 *  - Defaults variables
 *  - Clears the screen
 */
void vga_init(void) {
    kernel_log_info("Initializing VGA driver");

    if (vga_cursor) {
        // Enable the cursor
        vga_cursor_enable();
    } else {
        // Disable the cursor
        vga_cursor_disable();
    }

    // Clear the screen
    vga_clear();
}

/**
 * Sets the cursor position to the current VGA row/column (x/y)
 * position if the cursor is enabled.
 */
void vga_cursor_update(void) {
    if (vga_cursor) {
        unsigned short pos = vga_pos_x + vga_pos_y * VGA_WIDTH;

        outportb(VGA_PORT_ADDR, 0x0F);
        outportb(VGA_PORT_DATA, (unsigned char) (pos & 0xFF));

        outportb(VGA_PORT_ADDR, 0x0E);
        outportb(VGA_PORT_DATA, (unsigned char) ((pos >> 8) & 0xFF));
    }
}

/**
 * Enables the VGA text mode cursor
 */
void vga_cursor_enable(void) {
    vga_cursor = 1;

    // The cursor will be drawn between the scanlines defined
    // in the "Cursor Start Register" (0x0A) and the
    // "Cursor End Register" (0x0B)

    // To ensure that we do not change bits we are not intending,
    // read the current register state and mask off the bits we
    // want to save

    // Set the cursor starting scanline
    outportb(VGA_PORT_ADDR, 0x0A);
    outportb(VGA_PORT_DATA, (inportb(VGA_PORT_DATA) & 0xC0) | 0xE);

    // Set the cursor ending scanline
    // Ensure that bit 5 is not set so the cursor will be enabled
    outportb(VGA_PORT_ADDR, 0x0B);
    outportb(VGA_PORT_DATA, (inportb(VGA_PORT_DATA) & 0xE0) | 0xF);
}

/**
 * Disables the VGA text mode cursor
 */
void vga_cursor_disable(void) {
    vga_cursor = 0;

    // The cursor can be disabled by setting bit 5 in the "Cursor Start Register" (0xA)
    outportb(VGA_PORT_ADDR, 0x0A);
    outportb(VGA_PORT_DATA, 0x20);
}

/**
 * Clears the VGA output and sets the background and foreground colors
 */
void vga_clear(void) {
    unsigned short *vga_buf = VGA_BASE;

    for (unsigned int i = 0; i < (VGA_WIDTH * VGA_HEIGHT); i++) {
        vga_buf[i] = VGA_CHAR(vga_color_bg, vga_color_fg, 0x00);
    }

    vga_set_xy(0, 0);
}

/**
 * Sets the current X/Y (column/row) position
 *
 * @param x - x position (0 to VGA_WIDTH-1)
 * @param y - y position (0 to VGA_HEIGHT-1)
 * @notes If the input parameters exceed the valid range, the position
 *        will be set to the range boundary (min or max)
 */
void vga_set_xy(int x, int y) {

    if(x < 0){
        vga_pos_x = 0;
    }
    else if(x > VGA_WIDTH -1){
        vga_pos_x = VGA_WIDTH -1;
    }
    else{
        vga_pos_x = x;
    }

    if(y < 0){
        vga_pos_y = 0;
    }
    else if(y > VGA_HEIGHT -1){
        vga_pos_y = VGA_HEIGHT -1;
    }
    else{
        vga_pos_y = y;
    }

}

/**
 * Gets the current X (column) position
 * @return integer value of the column (between 0 and VGA_WIDTH-1)
 */
int vga_get_x(void) {
    return vga_pos_x;
}

/**
 * Gets the current Y (row) position
 * @return integer value of the row (between 0 and VGA_HEIGHT-1)
 */
int vga_get_y(void) {
    return vga_pos_y;
}

/**
 * Sets the background color.
 *
 * Does not modify any existing background colors, only sets it for
 * new operations.
 *
 * @param bg - background color
 */
void vga_set_bg(int bg) {
    vga_color_bg = bg;
}

/**
 * Gets the current background color
 * @return background color value
 */
int vga_get_bg(void) {
    return vga_color_bg;
}

/**
 * Sets the foreground/text color.
 *
 * Does not modify any existing foreground colors, only sets it for
 * new operations.
 *
 * @param color - background color
 */
void vga_set_fg(int fg) {
    vga_color_fg = fg;
}

/**
 * Gets the current foreground color
 * @return foreground color value
 */
int vga_get_fg(void) {
    return vga_color_fg;
}

/**
 * Prints the character on the screen.
 *
 * Does not change the x/y position, simply sets the character
 * at the current x/y position using existing background and foreground
 * colors. Does not change the cursor position.
 *
 * @param c - Character to print
 */
void vga_setc(char c) {
    
}

/**
 * Prints a character on the screen.
 *
 * When a character is printed, will do the following:
 *  - Update the x and y positions
 *  - If needed, will wrap from the end of the current line to the
 *    start of the next line
 *  - If the last line is reached, will ensure that all text is
 *    scrolled up
 *  - Special characters are handled as such:
 *    - tab character (\t) prints 'tab_stop' spaces
 *    - backspace (\b) character moves the character back one position,
 *      and prints a NULL character at that location.
 *
 * @param c - character to print
 */
void vga_putc(char c) {


}

/**
 * Prints a string on the screen.
 *
 * @param s - string to print
 */
void vga_puts(char *s) {
}

/**
 * Prints a character on the screen at the specified x/y position and
 * with the specified background/foreground colors
 *
 * Does not change the "current" x/y position
 * Does not change the "current" background/foreground colors
 *
 * @param x - x position (0 to VGA_WIDTH-1)
 * @param y - y position (0 to VGA_HEIGHT-1)
 * @param bg - background color
 * @param fg - foreground color
 * @param c - character to print
 */
void vga_putc_at(int x, int y, int bg, int fg, char c) {



}

/**
 * Prints a string on the screen at the specified x/y position and
 * with the specified background/foreground colors
 *
 * Does not change the "current" x/y position or background/foreground colors
 *
 * @param x - x position (0 to VGA_WIDTH-1)
 * @param y - y position (0 to VGA_HEIGHT-1)
 * @param bg - background color
 * @param fg - foreground color
 * @param s - string to print
 */
void vga_puts_at(int x, int y, int bg, int fg, char *s) {
}

