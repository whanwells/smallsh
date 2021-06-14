#include "cd.h"
#include <stdlib.h>
#include <unistd.h>

int cd(char *path)
{
    if (path == NULL) {
        return chdir(getenv("HOME"));
    } else {
        return chdir(path);
    }
}
