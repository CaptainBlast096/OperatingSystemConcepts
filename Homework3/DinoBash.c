/*
 Author: Jaleel Rogers
 Class: COP4610.04
 Date: 10/23/2024
 */

// C libraries
#include<stdio.h> //Basic input and output
#include<stdlib.h> //Using for exit
#include<string.h> //Manipulating strings
#include<unistd.h> //Provides access to POSIX
#include<sys/types.h> //Functionality of pid_t
#include<sys/wait.h> //Functionality of wait
#include<readline/readline.h> //Functionality of readline
#include<readline/history.h> //Functionality of history releated functions
#include<curses.h> //Provides a variety of colors

// Define commands
#define MAXCOM 1000 // Max number of letters supported
#define MAXLIST 100 // Max number of commands supported

// Greeting shell during startup
void motd()
{
    printf("\n\n\n\t*******Dino Shell*******");
       printf("             \n__\n");
       printf("              / _)\n");
       printf("     _.----._/ /\n");
       printf("    /         /\n");
       printf(" __/ (  | (  |\n");
       printf("/__.-'|_|--|_|\n");
    char* username = getenv("USER");
    printf("\n\n\nUSER: @%s", username);
    printf("\n");
    sleep(1);
}

// Function to take input
int userInput(char* str)
{
    char* buffer;
    printf("\033[0;32m");
    buffer = readline("\n>>> "); // readline reads a line of input
    printf("\033[0m");

    // Checking user input, if its not empty
    if (strlen(buffer) != 0) {
        add_history(buffer);
        strcpy(str, buffer);
        return 0;
    } else {
        return 1;
    }
}

// Function where the system command is executed
void execArgs(char** parsed)
{
    // Forking a child
    pid_t pid = fork();

    if (pid == -1) {
        printf("\nFailed forking child..");
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command..");
        }
        exit(0);
    } else {
        // waiting for child to terminate
        printf("\n------------------Starting program ----------\n");
        wait(NULL);
        printf("\n__________________Program ended --------------\n");
        return;
    }
}

// Function where the piped system commands is executed
void execArgsPiped(char** parsed, char** parsedPipe)
{
    int pipe_fd[2]; // 0 is read end, 1 is write end
    pid_t pid1, pid2; // Two procceses
    
    if (pipe(pipe_fd) < 0) { // Creates a pipe via systemcall
        printf("\nPipe could not be initialized");
        return;
    }
    
    pid1 = fork(); // First child fork
    
    if (pid1 < 0) { // In case the first fork fails
        printf("\nCould not fork");
        return;
    }

    if (pid1 == 0) { // Close the read end of pipe if its the first child
        // Child 1 executing...
        // It only needs to write at the write end
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);

        if (execvp(parsed[0], parsed) < 0) { // In case execution fails
            printf("\nCould not execute command 1..");
            exit(0);
        }
        
    } else {
        // Parent executing
        pid2 = fork();

        if (pid2 < 0) {
            printf("\nCould not fork");
            return;
        }

        // Child 2 executing..
        // It only needs to read at the read end
        if (pid2 == 0) {
            close(pipe_fd[1]);
            dup2(pipe_fd[0], STDIN_FILENO);
            close(pipe_fd[0]);

            if (execvp(parsedPipe[0], parsedPipe) < 0) {
                printf("\nCould not execute command 2..");
                exit(0);
            }
            
        } else {
            // Parent executing, waiting for two children
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            wait(NULL);
            wait(NULL);
        }
    }
}

// Function to execute builtin commands
int cmdHandler(char** parsed)
{
    int numCmds = 2, i, switchFlag = 0;
    char* ListOfOwnCmds[numCmds];
    ListOfOwnCmds[0] = "exit";
    ListOfOwnCmds[1] = "cd";

    for (i = 0; i < numCmds; i++) {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) {
            switchFlag = i + 1;
            break;
        }
    }

    switch (switchFlag) {
    case 1:
        printf("\nGoodbye...going extinct\n");
        exit(0);
    case 2:
        chdir(parsed[1]);
        return 1;
    default:
        break;
    }
    return 0;
}

// Function for finding pipe
int parsePipe(char* str, char** strpiped)
{
    int i;

    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, "|"); //Split string into substrings

        if (strpiped[i] == NULL) //When no more substrings need to be parsed
            break;
    }

    if (strpiped[1] == NULL)
        return 0; // returns zero if no pipe is found.

    else {
        return 1;
    }
}

// function for parsing command words
void parseSpace(char* str, char** parsed)
{
    int i;

    for (i = 0; i < MAXLIST; i++) {
        parsed[i] = strsep(&str, " "); //Split the string into substrings

        if (parsed[i] == NULL) //When there are no more substrings
            break;
        if (strlen(parsed[i]) == 0) //When parsed substring is empty
            i--;
    }
}

int processString(char* str, char** parsed, char** parsedpipe)
{

    char* strpiped[2];
    int piped = 0;

    piped = parsePipe(str, strpiped); //Checks if the input string contains a pipe character

    if (piped) {
        parseSpace(strpiped[0], parsed); //No pipe was found
        parseSpace(strpiped[1], parsedpipe); //Pipe was found

    } else {

        parseSpace(str, parsed); //Parses the entire input string
    }

    if (cmdHandler(parsed)) //Calls cmdHandler to handle passed arguments
        return 0;

    else
        return 1 + piped;
}

int main()
{
    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char* parsedArgsPiped[MAXLIST];
    int execFlag = 0;
    motd();

    while (1) {
        // print shell line
        // take input
        if (userInput(inputString))
            continue;
        // process
        execFlag = processString(inputString,
        parsedArgs, parsedArgsPiped);
        // execflag returns zero if there is no command
        // or it is a builtin command,
        // 1 if it is a simple command
        // 2 if it is including a pipe.

        // execute
        if (execFlag == 1)
            execArgs(parsedArgs);

        if (execFlag == 2)
            execArgsPiped(parsedArgs, parsedArgsPiped);
    }
    return 0;
}
