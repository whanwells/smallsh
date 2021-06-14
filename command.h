#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>

// command + 512 arguments
#define COMMAND_MAX_ARGS 513

// A command to be run by a process.
//
// - argv: an array of strings that represent the program argument list
// - argc: the number of arguments in argv
// - input: if not NULL, stdin will be redirected to this file
// - output: if not NULL, stdout will be redirected to this file
// - background: whether or not to run the command in the background
//
// See https://linux.die.net/man/3/execvp.
struct command {
    char *argv[COMMAND_MAX_ARGS];
    int argc;
    char *input;
    char *output;
    bool background;
};

// Initializes a command.
void command_init(struct command *command);

// Frees the memory allocated for a command's arguments and resets its counter.
void command_reset(struct command *command);

// Adds an argument to a command by pointing it to the given value.
//
// Returns true if the argument was added or false if adding it would exceed
// the command's capacity.
bool command_add_arg(struct command *command, char *value);

#endif
