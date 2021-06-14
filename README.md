# smallsh

A simple Linux shell written in C.

## Usage

```bash
command [arg1 arg2 ...] [< input_file] [> output_file] [&]
```

…where items in square brackets are optional.

## Features

- Blank lines and comments (lines beginning with `#`) are ignored
- `$$` is expanded to the process ID of the shell
- Input and output redirection with `<` and `>`
- Commands ending with `&` are run in the background (unless in foreground-only mode)

### Built-in Commands

> Note: All other commands are run as child processes of the shell.

#### `cd [path]`

Changes the working directory. Takes an optional absolute or relative path to change to. When called with no arguments, it changes to the directory specified by the `HOME` environment variable.

#### `exit`

Kills child processes or jobs started by the shell before terminating itself.

#### `status`

Prints either the exit status or the terminating signal of the last foreground process run by your shell.

### Signal Handlers

#### `SIGINT (Ctrl-C)`

Ignored by all processes except background children, which will terminate themselves upon receipt of the signal.

#### `SIGTSTP (Ctrl-Z)`

Only handled by the parent process, which will toggle foreground-only mode on and off.
