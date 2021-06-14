#include "status.h"
#include <stdio.h>
#include <sys/wait.h>

void status(int value)
{
    if (WIFEXITED(value)) {
        printf("exit value %d\n", WEXITSTATUS(value));
    } else {
        printf("terminated by signal %d\n", WTERMSIG(value));
    }

    fflush(stdout);
}
