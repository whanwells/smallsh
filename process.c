#include "process.h"
#include "cd.h"
#include "status.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// use a global variable to track the foreground child process ID (ugh)
static pid_t child_pid = 0;

// use a global variable to track foreground-only mode (ew)
static bool foreground_only_mode = false;

static void spawn(struct process *, struct command *);
static void redirect_input(char *);
static void redirect_output(char *);
static void add_child(struct process *, pid_t);
static void remove_child(struct process *, pid_t);

void process_init(struct process *process)
{
    process->status = 0;
    process->running = true;
    process->head = NULL;
    process->tail = NULL;
}

void process_run_command(struct process *process, struct command *command)
{
    char *name = command->argv[0];

    if (strcmp(name, "cd") == 0) {
        // attempt to change directory
        if (cd(command->argv[1]) != 0) {
            perror("cd");
            fflush(stderr);
        }
    } else if (strcmp(name, "exit") == 0) {
        // exiting program, kill any remaining background processes
        process->running = false;
        process_background_kill(process);
    } else if (strcmp(name, "status") == 0) {
        // print the last recorded status
        status(process->status);
    } else {
        // spawn a child process
        spawn(process, command);
    }
}

void process_background_check(struct process *process)
{
    struct child *child = process->head;

    while (child != NULL) {
        int status;
        pid_t pid = child->pid;
        child = child->next;

        if (waitpid(pid, &status, WNOHANG) > 0) {
            // child has exited or been terminated
            // print its status and remove it from the process list
            if (WIFEXITED(status)) {
                printf("background pid %d is done: exit value %d\n", pid, WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("background pid %d is done: terminated by signal %d\n", pid, WTERMSIG(status));
            }
            remove_child(process, pid);
        }
    }
}

void process_background_kill(struct process *process)
{
    // kill all child processes and remove them from the process list
    struct child *child = process->head;

    while (child != NULL) {
        pid_t pid = child->pid;
        child = child->next;
        kill(pid, SIGKILL);
        remove_child(process, pid);
    }
}

void process_handle_sigtstp()
{
    waitpid(child_pid, NULL, 0);

    // toggle foreground-only mode
    foreground_only_mode = !foreground_only_mode;

    if (foreground_only_mode) {
        char *message = "\nEntering foreground-only mode (& is now ignored)\n";
        write(STDOUT_FILENO, message, 51);
    } else {
        char *message = "\nExiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 31);
    }
}

// Based on the "Process API - Executing a New Program", "Processes and I/O",
// and "Signal Handling API" explorations.
static void spawn(struct process *process, struct command *command)
{
    pid_t pid = fork();

    switch (pid) {
    case -1:
        // womp womp
        perror("fork failed\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
        break;
    case 0:
        // child process
        // redirect input and output if needed
        if (command->input != NULL) {
            redirect_input(command->input);
        }

        if (command->output != NULL) {
            redirect_output(command->output);
        }

        // Ignore SIGTSTP
        struct sigaction sigtstp;
        memset(&sigtstp, 0, sizeof(struct sigaction));
        sigtstp.sa_handler = SIG_IGN;
        sigaction(SIGTSTP, &sigtstp, NULL);

        if (!command->background) {
            // allow foreground child process to receive SIGINT
            struct sigaction sigint;
            memset(&sigint, 0, sizeof(struct sigaction));
            sigfillset(&sigint.sa_mask);
            sigint.sa_handler = SIG_DFL;
            sigaction(SIGINT, &sigint, NULL);
        }

        execvp(command->argv[0], command->argv);
        perror(command->argv[0]);
        fflush(stderr);
        _exit(EXIT_FAILURE);
        break;
    default:
        // parent process
        if (command->background && !foreground_only_mode) {
            // run child in background and add it to the process list
            waitpid(pid, NULL, WNOHANG);
            printf("background pid is %d\n", pid);
            fflush(stdin);
            add_child(process, pid);
        } else {
            // set global child_pid so SIGTSTP can wait for it to complete
            child_pid = pid;

            // wait for the process to complete
            waitpid(child_pid, &process->status, 0);

            if (WIFSIGNALED(process->status)) {
                // notify for terminated processes
                printf("terminated by signal %d\n", WTERMSIG(process->status));
                fflush(stdin);
            }
        }
    }
}

static void redirect_input(char *path)
{
    int fd = open(path, O_RDONLY);

    if (fd == -1) {
        fprintf(stderr, "cannot open %s for input\n", path);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    if (dup2(fd, 0) == -1) {
        perror("dup2");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
}

static void redirect_output(char *path)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0777);

    if (fd == -1) {
        fprintf(stderr, "cannot open %s for output\n", path);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    if (dup2(fd, 1) == -1) {
        perror("dup2");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
}

static void add_child(struct process *process, pid_t pid)
{
    struct child *child = malloc(sizeof(struct child));
    child->pid = pid;
    child->next = NULL;

    if (process->head == NULL) {
        process->head = child;
        process->tail = child;
    } else {
        process->tail->next = child;
        process->tail = child;
    }
}

static void remove_child(struct process *process, pid_t pid)
{
    struct child *parent = NULL;
    struct child *child = process->head;

    // find the child with the given pid and free its memory
    while (child != NULL) {
        if (child->pid == pid) {
            if (parent == NULL) {
                // child is head
                process->head = child->next;
            } else {
                // child is not head
                parent->next = child->next;
            }
            free(child);
            break;
        }
        parent = child;
        child = child->next;
    }
}
