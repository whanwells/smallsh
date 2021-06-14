#ifndef STATUS_H
#define STATUS_H

// Prints out either the exit status or the terminating signal by interpreting
// the given value.
//
// See https://man7.org/linux/man-pages/man0/sys_wait.h.0p.html.
void status(int value);

#endif
