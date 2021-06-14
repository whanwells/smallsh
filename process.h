#ifndef PROCESS_H
#define PROCESS_H

#include "command.h"
#include <stdbool.h>
#include <sys/types.h>

// Monitors a running process.
//
// - status: the last status set by a child process
// - running: whether or not the process is running
// - head: the first node in a linked list of child processes
// - tail: the last node in a linked list of child processes
struct process {
    int status;
    bool running;
    struct child *head;
    struct child *tail;
};

// A child process.
//
// - pid: the process ID of the process
// - next: the next node in a linked list of children
struct child {
    pid_t pid;
    struct child *next;
};

// Initializes a process.
void process_init(struct process *process);

// Runs a command and updates the given process.
void process_run_command(struct process *process, struct command *command);

// Checks the status of background processes.
void process_background_check(struct process *process);

// Kills any running background processes.
void process_background_kill(struct process *process);

// SIGTSTP handler that toggles foreground-only mode.
void process_handle_sigtstp();

#endif
