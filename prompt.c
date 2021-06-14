#include "prompt.h"
#include <stdio.h>
#include <string.h>

char *prompt(char *buffer)
{
    printf(": ");

    if (fgets(buffer, PROMPT_INPUT_LENGTH, stdin) == NULL) {
        // error or EOF occured while no characters have been read
        return NULL;
    }

    size_t last = strlen(buffer) - 1;

    if (buffer[last] == '\n') {
        // replace newline with null terminating byte
        buffer[last] = '\0';
    }

    return buffer;
}
