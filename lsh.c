/*
lsh.c - Simple Linux Shell
Author: Gerell Perrington
Compile: gcc -o lsh lsh.c
Run:  .lsh
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define LSH_BUFSIZE 1024
#define LSH_TOKSIZE 64
#define LSH_DELIM " \t\r\n\a"

// Read input
char *lsh_read_line(void){
    int bufsize = LSH_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char)* bufsize);
    int c;

    if (!buffer){
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);

    }

    while (1){
        c = getchar();

        if (c == EOF || c== '\n'){
            buffer[position] = '\0';
            return buffer;
        }

        buffer[position++] = (char)c;

    }
}

// Tokenize input
char **lsh_split_line(char *line){
    int bufsize = LSH_TOKSIZE;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if(!tokens){
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_DELIM);
    while (token != NULL){
        tokens[position++] = token;
        token = strtok(NULL, LSH_DELIM);
    }

    tokens[position] = NULL;
    return tokens;

}

//Built in cd
int lsh_cd(char **args){
    if (args[1] ==NULL){
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else if (chdir (args[1]) != 0){
        perror("lsh");
    }
    return 1;
}

//Help
int lsh_help(char **args){
    (void)args;

    printf("\n  LSH - Simple Linux Shell\n");
    printf("  Author: Gerell Perrington Jr.\n\n");
    printf("  Built-in commands:\n");
    printf("    cd <path>  -- change directory\n");
    printf("    help       -- show this message\n");
    printf("    exit       -- quit the shell\n");
    printf("\n  Any other command runs as an external program.\n\n");
    return 1;
}

//Exit
int lsh_exit(char **args){
    (void)args;
    return 0;
}

//Table
char *builtin_str[] = {"cd", "help", "exit"};

int (*builtin_func[])(char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};

int lsh_num_builtins(void){
    return sizeof(builtin_str) / sizeof(char *);
}

//launch external command
int lsh_launch(char **args){
    pid_t pid;
    int status;

    pid = fork();

    if (pid == 0){
        //Child process - runs the command
        if (execvp(args[0], args) == -1){
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    }else if (pid < 0){
        //Fork failed
        perror("lsh");
    }else {
        //Parent - waits for the child to finish
        waitpid(pid, &status, WUNTRACED);
    }
    return 1;
}

//Execute
int lsh_execute(char **args){
    int i;

    if (args[0] == NULL){
        return 1;
    }

    for (i = 0; i < lsh_num_builtins(); i++){
        if (strcmp(args[0], builtin_str[i]) == 0){
            return (*builtin_func[i])(args);
        }
    }

    return lsh_launch(args);
}

//Main
int main(void) {
    char *line;
    char **args;
    int status;

    do{
    printf("lsh> ");
    fflush(stdout);

    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    //lsh_launch(args);

   /* printf("Tokens:\n");
    for(i = 0; args[i] != NULL; i++){
        printf(" [%d] %s\n", i, args[i]);
    }*/

    free(line);
    free(args);
    }while(status);

    return 0;
}