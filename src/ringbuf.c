/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * Fall 2022
 *
 * Simple ring buffer implementation
 */

#include <spede/stdbool.h>      // for bool type
#include <spede/stddef.h>       // for size_t
#include "ringbuf.h"

/**
 * Initializes an empty ring buffer
 * Sets the empty data to 0
 *
 * @param  buf - pointer to the ring buffer data structure
 * @return -1 on error; 0 on success
 */
int ringbuf_init(ringbuf_t *buf) {

    if (!buf)
        return -1;

    buf->head = -1;
    buf->tail = -1;
    buf->size = 0;

    return 0;
}

/**
 * Writes a byte to the buffer
 * @param  buf   - pointer to the ring buffer structure
 * @param  byte  - the byte to write
 * @return -1 on error; 0 on success
 */
int ringbuf_write(ringbuf_t *buf, char byte) {
    if (ringbuf_is_full(buf)) {
        return -1;
    } else {
        if (buf->head == -1) {
            buf->head = 0;
        }
        buf->size++;
        buf->tail = (buf->tail + 1) % RINGBUF_SIZE;
        buf->data[buf->tail] = byte;
    }
    return 0;
}

/**
 * Reads a byte from the buffer
 * @param  buf - pointer to the ring buffer structure
 * @param  byte - pointer to the byte to be stored
 * @return -1 on error; 0 on success
 */
int ringbuf_read(ringbuf_t *buf, char *byte) {
    if (ringbuf_is_empty(buf)) {
        return -1;
    } 
    else {

        *byte = buf->data[buf->head];
        buf->size--;
        if (buf->head == buf->tail) {
            buf->head = -1;
            buf->tail = -1;
        } else {
            buf->head = (buf->head + 1) % RINGBUF_SIZE;
        }
        return 0;
    }
}

/**
 * Copies multiple bytes to the buffer from the specified memory
 * @param buf - pointer to the ring buffer structure
 * @param mem - pointer to the memory location to copy from
 * @param size - number of bytes to copy
 * @return -1 on error, 0 on success
 * @note Should return an error if the number of bytes
 *       cannot be copied - i.e. the buffer would overflow
 */
int ringbuf_write_mem(ringbuf_t *buf, char *mem, size_t size) {
    int i =0;
    while (i < (int) size){
	if (!ringbuf_is_full(buf))
	    ringbuf_write(buf, mem[i]);
    }
    return 0;
}

/**
 * Copies multiple bytes from the buffer to the specified memory
 * @param buf - pointer to the ring buffer structure
 * @param mem - pointer to the memory location to copy to
 * @param size - number of bytes to copy
 * @return -1 on error, positive value to indicate number of bytes
 *         copied
 */
int ringbuf_read_mem(ringbuf_t *buf, char *mem, size_t size) {
    int i =0;
    while (i < (int)size){
        if (!ringbuf_is_empty(buf))
            ringbuf_read(buf, mem);
    }

    return 0;
}

/**
 * Flushes (empties) the buffer
 * @param buf - pointer to the ring buffer structure
 * @return -1 on error, 0 on success
 */
int ringbuf_flush(ringbuf_t *buf) {
    int i = 0;
    while (i < buf->size){
	if (!ringbuf_is_full(buf))
	    ringbuf_read(buf, "");
    }
    return 0;
}

/**
 * Indicates if the buffer is empty
 * @param buf - pointer to the ring buffer structure
 * @return true if empty, false if not empty
 */
bool ringbuf_is_empty(ringbuf_t *buf) {
    if (buf->size == 0)
        return true;

    return false;
}

/**
 * Indicates if the buffer if full
 * @param buf - pointer to the ring buffer structure
 * @return true if full, false if not full
 */
bool ringbuf_is_full(ringbuf_t *buf) {
    if ((buf->head == buf->tail + 1) || (buf->head == 0 && buf->tail == RINGBUF_SIZE - 1)) {
        return true;
    }

    return false;
}

