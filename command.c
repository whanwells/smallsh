#include "command.h"
#include <stdlib.h>
#include <string.h>

void command_init(struct command *command)
{
    for (int i = 0; i < COMMAND_MAX_ARGS; i++) {
        command->argv[i] = NULL;
    }
    command->argc = 0;
    command->input = NULL;
    command->output = NULL;
    command->background = false;
}

void command_reset(struct command *command)
{
    for (int i = 0; i < command->argc; i++) {
        free(command->argv[i]);
        command->argv[i] = NULL;
    }

    free(command->input);
    free(command->output);

    command->argc = 0;
    command->input = NULL;
    command->output = NULL;
    command->background = false;
}

bool command_add_arg(struct command *command, char *value)
{
    if (command->argc == COMMAND_MAX_ARGS) {
        return false;
    }

    command->argv[command->argc] = value;
    command->argc++;
    return true;
}
