#define _GNU_SOURCE
#include "parser.h"
#include "process.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *getpids();
static char *expand(char *, int, char *l, size_t);

struct command *parse(char *str, struct command *command)
{
    command_reset(command);

    // current position in string
    int pos = 0;

    // token starting position
    int start = 0;

    // token length
    int len = 0;

    // process ID string for variable expansion
    char *pids = getpids();
    size_t p_len = strlen(pids);

    // the previous character
    char prev = 0;

    // parse arguments by default
    bool args = true;

    // whether we're parsing an input or output file
    bool input = false;
    bool output = false;

    while (str[pos] != '\0') {
        char curr = str[pos];
        char next = str[pos + 1];

        if (!isspace(curr)) {
            if (curr == '#' && command->argc == 0) {
                // a comment is beginning, bail out
                break;
            }

            if (isspace(prev)) {
                if (curr == '&') {
                    // stop parsing and set command to run in background
                    // input and output will redirect to /dev/null if NULL
                    command->background = true;

                    if (command->input == NULL) {
                        command->input = strdup("/dev/null");
                    }

                    if (command->output == NULL) {
                        command->output = strdup("/dev/null");
                    }
                    break;
                } else if (isspace(next)) {
                    if (curr == '<' && command->input == NULL) {
                        // begin parsing input file
                        args = false;
                        input = true;
                        output = false;
                        pos++;
                        continue;
                    } else if (curr == '>' && command->output == NULL) {
                        // begin parsing output file
                        args = false;
                        input = false;
                        output = true;
                        pos++;
                        continue;
                    }
                }
            }

            if (len == 0) {
                // if the token is empty, set it's starting position
                start = pos;
            }

            len++;

            if (isspace(next) || next == '\0') {
                // if next character is whitespace or terminating null byte,
                // the token is complete
                char *token = expand(&str[start], len, pids, p_len);

                if (args) {
                    command_add_arg(command, token);
                } else if (input) {
                    command->input = token;
                    input = false;
                } else if (output) {
                    command->output = token;
                    output = false;
                }

                len = 0;
            }
        }

        prev = curr;
        pos++;
    }

    free(pids);

    return command->argc > 0 ? command : NULL;
}

static char *getpids()
{
    char *pids;
    asprintf(&pids, "%d", getpid());
    return pids;
}

static char *expand(char *src, int n, char *pids, size_t p_len)
{
    // count occurences of '$$'
    int count = 0;

    for (int i = 0; i < n; i++) {
        char curr = src[i];
        char next = i + 1 < n ? src[i + 1] : 0;

        if (curr == '$' && next == '$') {
            count++;
            i++;
        }
    }

    // allocate memory to fit expanded variables
    size_t d_len = n + ((p_len - 2) * count);
    char *dest = malloc(d_len + 1);

    // source and destination positions
    int s_pos = 0;
    int d_pos = 0;

    while (s_pos < n) {
        char curr = src[s_pos];
        char next = s_pos + 1 < n ? src[s_pos + 1] : 0;

        if (curr == '$' && next == '$') {
            // expand variable
            memcpy(dest + d_pos, pids, p_len);
            s_pos += 2;
            d_pos += p_len;
        } else {
            // copy as-is
            dest[d_pos] = curr;
            s_pos++;
            d_pos++;
        }
    }

    // add terminating null byte to end of string
    dest[d_len] = '\0';
    return dest;
}
