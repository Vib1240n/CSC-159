/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * Fall 2022
 *
 * Simple circular queue implementation
 */

#include <spede/stdbool.h>
#include "queue.h"

/**
 * Initializes an empty queue
 * Sets the empty queue items to -1
 *
 * @param  queue - pointer to the queue
 * @return -1 on error; 0 on success
 */
int queue_init(queue_t *queue) {

    if (!queue)
        return -1;

    queue->head = -1;
    queue->tail = -1;
    queue->size = 0;

    return 0;
}

/**
 * Adds an item to the end of a queue
 * @param  queue - pointer to the queue
 * @param  item  - the item to add
 * @return -1 on error; 0 on success
 */
int queue_in(queue_t *queue, int item) {
    if (queue_is_full(queue)) {
        return -1;
    } else {
        if (queue->head == -1) {
            queue->head = 0;
        }

        queue->tail = (queue->tail + 1) % QUEUE_SIZE;


        queue->items[queue->tail] = item;


    }

    return 0;
}

/**
 * Pulls an item from the specified queue
 * @param  queue - pointer to the queue
 * @param  item  - pointer to the memory to save item to
 * @return -1 on error; 0 on success
 */
int queue_out(queue_t *queue, int *item) {

    if (queue_is_empty(queue)) {
        return -1;
    } else {

        *item = queue->items[queue->head];
        if (queue->head == queue->tail) {
            queue->head = -1;
            queue->tail = -1;
        } else {
            queue->head = (queue->head + 1) % QUEUE_SIZE;
        }


        return 0;
    }
}

/**
 * Indicates if the queue is empty
 * @param queue - pointer to the queue structure
 * @return true if empty, false if not empty
 */
bool queue_is_empty(queue_t *queue) {
    if (queue->head == -1)
        return true;
    return false;
}

/**
 * Indicates if the queue if full
 * @param queue - pointer to the queue structure
 * @return true if full, false if not full
 */
bool queue_is_full(queue_t *queue) {
    if ((queue->head == queue->tail + 1) || (queue->head == 0 && queue->tail == QUEUE_SIZE - 1)) {
        return true;
    }

    return false;
}

