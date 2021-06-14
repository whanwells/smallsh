#ifndef PARSER_H
#define PARSER_H

#include "command.h"

// Constructs a command by parsing a string until a terminating null byte is
// found. The command will be reset before parsing begins to ensure it starts
// in a fresh state. All instances of "$$" will be expanded to the current
// process ID.
//
// Returns the command on success, and NULL if the string contains a comment or
// only whitespace. A string is considered a comment if the first non-whitespace
// character is '#'.
struct command *parse(char *str, struct command *command);

#endif
