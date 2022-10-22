/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * Fall 2022
 *
 * Kernel Process Handling
 */

#include <spede/string.h>
#include <spede/stdio.h>
#include <spede/time.h>
#include <spede/machine/proc_reg.h>

#include "kernel.h"
#include "kproc.h"
#include "scheduler.h"
#include "timer.h"

#include "queue.h"

// Process Queues
queue_t run_queue;

/**
 * Scheduler timer callback
 */
void scheduler_timer(void) {
    // Update the active process' run time and CPU time
    active_proc->run_time++;
    active_proc->cpu_time++;
}

/**
 * Executes the scheduler
 * Should ensure that `active_proc` is set to a valid process entry
 */
void scheduler_run(void) {
    // Ensure that processes not in the active state aren't still scheduled
    
    active_proc = pid_to_proc(0);

    // Check if we have an active process

    if (active_proc) {
        // Check if the current process has exceeded it's time slice
        if (active_proc->run_time >= SCHEDULER_TIMESLICE) {
            // Reset the active time
            active_proc->run_time = 0;
            // If the process is not the idle task, add it back to the scheduler
            // Otherwise, simply set the state to IDLE
            if (active_proc->state != IDLE) {
                scheduler_add(active_proc);
            }
            else {
                active_proc->state = IDLE;
            }
            // Unschedule the active process
            active_proc = NULL;
        }
    }
    // Check if we have a process scheduled or not
    //if (
        // Get the proces id from the run queue
            // default to process id 0 (idle task) if a process can't be scheduled

        // Update the active proc pointer

    // Make sure we have a valid process at this point

    // Ensure that the process state is set
}

/**
 * Adds a process to the scheduler
 * @param proc - pointer to the process entry
 */
void scheduler_add(proc_t *proc) {
    // Add the process to the run queue
    queue_in(&run_queue, proc->pid);
    // Set the process state
    proc->state = ACTIVE;
    kernel_log_debug("Scheduling process pid=%d, name=%c", active_proc->pid, active_proc->name);
}

/**
 * Removes a process from the scheduler
 * @param proc - pointer to the process entry
 */
void scheduler_remove(proc_t *proc) {
    // Iterate through each the process queue
    // If the processis found, skip it; otherwise, ensure that each other process remains in the queue
    // If the process is the active process, ensure that the active process is cleared so when the
    // scheduler runs again, it will select a new process to run
    if(queue_out(&run_queue, &proc->pid)) {
        if (proc->state == ACTIVE) {
            active_proc = NULL;
        }
        else {
            queue_in(&run_queue, proc->pid);
        }
    }
}

/**
 * Initializes the scheduler, data structures, etc.
 */
void scheduler_init(void) {
    kernel_log_info("Initializing scheduler");

    // Initialize any data structures or variables
    //active_proc->pid = pid_to_proc(0);
    // Register the timer callback (scheduler_timer) to run every tick
    timer_callback_register(scheduler_timer, 2, -1);
}

