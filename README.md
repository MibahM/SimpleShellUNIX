# SimpleShellUNIX

This project is coded in C to be run in a UNIX/Linux system.

Use this line to compile: 

  gcc sish.c –o sish -Wall -Werror -std=gnu99

There should be no warnings or errors. 


The goal of this project is to get a deeper grasp on UNIX and shells using forks, piping, and custom built-in functions that function similarly to Bash commands.

This project involves implementing a basic command line interpreter or shell called "sish" that operates similarly to the Linux bash shell. The shell creates child processes to execute commands entered by the user and displays a prompt for further input upon completion. 

  Features:

1. Simple Shell:
   - The shell executable is named "sish" with the source code in "sish.c".
   - Displays a continuous prompt "sish>" for user input.
   - Parses input commands and their arguments.
   - Executes commands either as executables or built-in shell commands (for Part 2).

2. Built-in Commands:
   - Implements built-in commands: exit, cd, history.
     - "exit": Exits the shell.
     - "cd [dir]": Changes the current working directory.
     - "history [-c] [offset]": Displays past commands, clears history, or executes a command from history.

3. Pipes:
   - Enhances shell to execute a sequence of programs communicating through pipes.
   - Supports multiple processes communicating through pipes, not limited to two.
   - Does not support built-in commands with pipes.

Implementation Details:
- Parsing of commands and arguments, execution of executables using fork() and execvp().
- Built-in commands implemented using system calls like chdir().
- Support for pipes between processes achieved using pipe file descriptors and dup2().

This project provides a simplified version of a command line interpreter with essential functionalities. 
