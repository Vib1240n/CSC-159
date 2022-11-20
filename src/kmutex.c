/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * Fall 2022
 *
 * Kernel Mutexes
 */

#include <spede/string.h>

#include "kernel.h"
#include "kmutex.h"
#include "queue.h"
#include "scheduler.h"

// Table of all mutexes
mutex_t mutexes[MUTEX_MAX];

// Mutex ids to be allocated
queue_t mutex_queue;

/**
 * Initializes kernel mutex data structures
 * @return -1 on error, 0 on success
 */
int kmutexes_init() {
    kernel_log_info("Initializing kernel mutexes");

    // Initialize the mutex table
    memset(&mutexes, 0, sizeof(mutexes));
    for (int i = 0; i < MUTEX_MAX; i++) {
        mutexes[i].allocated = 0;
    }
    // Initialize the mutex queue
    queue_init(&mutex_queue);
    // Fill the mutex queue
    for (int i = 0; i < MUTEX_MAX; i++) {
        if (queue_in(&mutex_queue, i) != 0) {
            kernel_log_warn("timer: unable to queue mutex %d", i);
        }
    }

    return 0;
}

/**
 * Allocates a mutex
 * @return -1 on error, otherwise the mutex id that was allocated
 */
int kmutex_init(void) {
    // Obtain a mutex id from the mutex queue
    int id = -1;
    if (queue_out(&mutex_queue, &id) != 0) {
        id++;
    }
    // Ensure that the id is within the valid range
    if(id > MUTEX_MAX || id < 0) {
        return -1;
    }
    // Pointer to the mutex table entry
    mutex_t *mutex = &mutexes[id];
    // Initialize the mutex data structure (mutex_t + all members)
    mutex->allocated = 1;
    mutex->locks = 0;
    mutex->owner = NULL;
    memset(&mutex->wait_queue, 0, sizeof(mutex->wait_queue));
    // return the mutex id
    return id;
}

/**
 * Frees the specified mutex
 * @param id - the mutex id
 * @return 0 on success, -1 on error
 */
int kmutex_destroy(int id) {
    // look up the mutex in the mutex table
    if(id > MUTEX_MAX || id < 0) {
        return -1;
    }
    mutex_t *mutex = &mutexes[id];
    // If the mutex is locked, prevent it from being destroyed (return error)
    if (mutex->owner != NULL) {
        return -1;
    }
    // Add the id back into the mutex queue to be re-used later
    queue_in(&mutex_queue, id);
    // Clear the memory for the data structure
    mutex->allocated = 0;
    mutex->locks = 0;
    mutex->owner = NULL;
    memset(&mutex->wait_queue, 0, sizeof(mutex->wait_queue));

    return 0;
}

/**
 * Locks the specified mutex
 * @param id - the mutex id
 * @return -1 on error, otherwise the current lock count
 */
int kmutex_lock(int id) {
    // look up the mutex in the mutex table
    mutex_t *mutex = &mutexes[id];
    // If the mutex is already locked
    //   1. Set the active process state to WAITING
    //   2. Add the process to the mutex wait queue (so it can take
    //      the mutex when it is unlocked)
    //   3. Remove the process from the scheduler, allow another
    //      process to be scheduled
    if (mutex->owner != NULL) {
        active_proc->state = WAITING;
        if (queue_in(&mutex->wait_queue, active_proc->pid) != 0) {
            return -1;
        }
        scheduler_remove(active_proc);
    }
    // If the mutex is not locked
    //   1. set the mutex owner to the active process
    if (mutex->owner == NULL) {
        mutex->owner = active_proc;
    }
    // Increment the lock count
    mutex->locks = mutex->locks + 1;
    // Return the mutex lock count
    return mutex->locks;
}

/**
 * Unlocks the specified mutex
 * @param id - the mutex id
 * @return -1 on error, otherwise the current lock count
 */
int kmutex_unlock(int id) {
    // look up the mutex in the mutex table
    mutex_t *mutex = &mutexes[id];
    // If the mutex is not locked, there is nothing to do
    /*
    if (mutex->owner == NULL) {
        return -1;
    }
    */
    // Decrement the lock count
    mutex->locks = mutex->locks - 1;
    // If there are no more locks held:
    //    1. clear the owner of the mutex
    if (mutex->locks <= 0) {
        mutex->owner = NULL;
    }
    // If there are still locks held:
    //    1. Obtain a process from the mutex wait queue
    //    2. Add the process back to the scheduler
    //    3. set the owner of the of the mutex to the process
    if (mutex->locks > 0) {
        for (int i = 0; i < MUTEX_MAX; i++) {
            if (queue_out(&mutex->wait_queue, &i) != 0) {
                queue_in(&mutex->wait_queue, i);
            }
            else {
                scheduler_add(pid_to_proc(i));
                mutex->owner = pid_to_proc(i);
                break;
            }
        }
    }
    // return the mutex lock count
    return mutex->locks;
}
