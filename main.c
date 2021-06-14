#include "command.h"
#include "parser.h"
#include "process.h"
#include "prompt.h"
#include <signal.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    struct process process;
    process_init(&process);

    // Ignore SIGINT
    struct sigaction sigint;
    memset(&sigint, 0, sizeof(struct sigaction));
    sigint.sa_handler = SIG_IGN;
    sigaction(SIGINT, &sigint, NULL);

    // Handle SIGTSTP
    struct sigaction sigtstp;
    memset(&sigtstp, 0, sizeof(struct sigaction));
    sigfillset(&sigtstp.sa_mask);
    sigtstp.sa_handler = process_handle_sigtstp;
    sigaction(SIGTSTP, &sigtstp, NULL);

    char input[2050];
    struct command command;
    command_init(&command);

    while (process.running) {
        if (prompt(input) != NULL) {
            if (parse(input, &command) != NULL) {
                process_run_command(&process, &command);
            }

            // check the status of background processes and remove any
            // completed ones from the process list
            process_background_check(&process);
        }
    }

    // cleanup command memory
    command_reset(&command);

    return EXIT_SUCCESS;
}
