#ifndef PROMPT_H
#define PROMPT_H

#define PROMPT_INPUT_LENGTH 2050

// Displays a prompt then reads at most 2049 characters from stdin into the
// given buffer, which must be at least 2050 characters to prevent overflows.
//
// Reading stops after an EOF or a newline. If a newline is read, it gets
// replaced in the buffer with a terminating null byte. A terminating null byte
// is stored after the last last character in the buffer.
//
// Returns the buffer on success, and NULL on error or when end of file occurs
// while no characters have been read.
//
// See https://linux.die.net/man/3/fgets.
char *prompt(char *buffer);

#endif
