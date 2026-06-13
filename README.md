# LSH - Simple Linux Shell

A custom command-line shell written in C, built from scratch on Linux/WSL.

## Author
Gerell Perrington Jr.

## How to Compile and Run
gcc -o lsh lsh.c
./lsh

## Built-in Commands
- cd <path> -- change directory
- help -- show available commands
- exit -- quit the shell

## How It Works
The shell reads user input, tokenizes it, then either handles it
as a built-in command or forks a child process and uses execvp()
to run it as an external program.