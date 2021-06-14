#ifndef CD_H
#define CD_H

// Attempts to change the working directory to the given path. If path is NULL,
// it attempts to change to the directory specified in the HOME environment
// variable.
//
// On success, zero is returned. On error, -1 is returned, and errno is set
// appropriately.
//
// See https://linux.die.net/man/2/chdir.
int cd(char *path);

#endif
