/*
 Author: Jaleel Rogers
 Class:
 Date:
 */
// C libraries
#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
//#include<sys/wait.h>
//#include<readline/readline.h>
//#include<readline/history.h>

// Define commands

void motd() {
    //printf("\033[0;34m"); supposed to change color
    printf("\n");
    printf("  Wecome to Dino Shell!\n");
    printf("-------------------------------\n");
    printf("Enter your commands below.\n");
    printf("Type 'exit' to quit.\n");
    char* username = getenv("USER");
    printf("USER is: @%s", username);
    printf("\n");
}

int userInput(char *str) {
    char* buffer;

    buffer = readline("\n>>>");
    if (strlen(buffer) != 0) {
        add_history(buffer);
        strcpy(str, buffer);
        return 0;
    }
    else {
        return 1;
    }
}
void execArgs(char **parsed) {
    pid_t pid = fork();

    if (pid == -1) {
        printf("\nFailed forking child...");
        return;
    }
    else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command...");
        }
        exit(0);
    } else {
        wait(NULL);
        return;
    }
}

int cd(char *path) {
    if (cd(path) != 0) {
        perror("chdir"); //Prints and error message
        return 1;
    }
    return 0;
}

int cmdHandler(char **parsed) {
    int numCmds = 2, i, switchFlag;
    char* username, commandList[numCmds];

    commandList[0] = "exit";
    commandList[1] = "cd";

    for (i = 0; i < numCmds; i++) {
        if (strcmp(parsed[0], commandList[i]) == 0) {
            switchFlag = i + 1;
            break;
        }
    }
    switch (switchFlag) {
        case 1:
            printf("\nExiting Dino Shell.\n");
            exit(0);
        case 2:
            cd(parsed[1]);
            return 1;
        default:
            break;
    }
    return 0;
}

void execArgsPiped(char** parsed, char** parsedPipe) {
    int pipe[2];
    pid_t pid1, pid2;

    if (pipe(pipe) < 0) {
        printf("\nPipe failed.\n");
        return;
    }
    pid1 = fork();
    if (pid1 < 0) {
        printf("\nFork failed.\n");
        return;
    }
    if (pid1 == 0) {
        close(pipe[0]);
        dup2(pipe[1], STDOUT_FILENO);
        close(pipe[1]);

        if (execvp(parsed[0], parsed) < 0) {
            printf("\nExec failed.\n");
            exit(0);
        }
    }
        else {
            pid2 = fork();

            if (pid2 < 0) {
                printf("\nFork failed.\n");
                return;
            }

            if (pid2 == 0) {
                close(pipe[1]);
                dup2(pipe[0], STDIN_FILENO);
                close(pipe[0]);
                if (execvp(parsedPipe[0], parsedPipe) < 0) {
                    printf("\nExec failed.\n");
                    exit(0);
                }
            }
            else {
                wait(NULL);
                wait(NULL);
            }
        }
    }
int parsePipe(char* str, char** strpiped) {
    int i;

    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
    }

        if (strpiped[1] == NULL)
            return 0;
        else {
            return 1;
        }
}
void parseSpace(char* str, char** parsed) {
    int i;

    for (i = 0; i < MAXLIST; i++) {
        parsed[i] = strsep(&str, " ");

        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
}

int processString(char* str, char** parsed, char** parsedPipe) {
    char* strpiped[2];
    int piped = 0;

    piped = parsePipe(str, strpiped);

    if (piped) {
        parseSpace(strpiped[0], parsed);
        parseSpace(strpiped[1], parsedPipe);
    }
    else {
        parseSpace(str, parsed);
    }
    if (cmdHandler(parsed))
        return 0;
    else
        return 1 + piped;
}


int main(void) {
    motd();
    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char* parsedArgsPiped[MAXLIST];
    int execFlag = 0;
    init_shell();

    while (1) {
        execFlag = processString(inputString, parsedArgs, &parsedArgsPiped);

        if (execFlag == 1)
            execArgs(parsedArgs);

        if (execFlag == 2)
            execArgsPiped(parsedArgs, parsedArgsPiped);
    }
    return 0;
}
