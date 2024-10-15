/*
 Author: Jaleel Rogers
 Class: COP4610.04
 Date: 10/23/2024
 */

//C libraries
#include<stdio.h> //Basic input and output
#include<stdlib.h> //Using for exit
#include<string.h> //Manipulating strings
#include<unistd.h> //Provides access to POSIX
#include<sys/types.h> //Functionality of pid_t
#include<sys/wait.h> //Functionality of wait
#include<readline/readline.h> //Functionality of readline
#include<readline/history.h> //Functionality of history releated functions
#include<curses.h> //Provides a variety of colors

//Define commands
#define MAXCOM 1000 //Max number of letters supported
#define MAXLIST 100 //Max number of commands supported

//Startup greeting
void motd() {
    attron(COLOR_PAIR(2)); //Sets the current foreground color
    printf("\n\n\n\n******************"
        "************************");
    printf("\n\n\n\t****Dino Shell****");
    printf("\n\n\n\n*******************"
        "***********************");
    char* username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    sleep(1);
    clear();
    attroff(COLOR_PAIR(2)); // Resets the color attributes to default values
}

//Reads a line of input from the user and stores it in a buffer along with history
int userInput(char* str) {
    char* buffer;
    attron(COLOR_PAIR(1));
    buffer = readline("\n>>>"); //readline reads a line of input
    attroff(COLOR_PAIR(1));

    // Checking user input, if its not empty
    if (strlen(buffer) != 0) {
        add_history(buffer);
        strcpy(str, buffer);
        return 0;
    }
    else {
        return 1;
    }
}

//System command is executed
void execArgs(char** parsed) {
    pid_t pid = fork(); //Forks a child
    attron(COLOR_PAIR(2));

    if (pid == -1) { //In case there is an error
        printf("\nFailed forking child...");
        return;
    }
    else if (pid == 0) { //Current child process
        if (execvp(parsed[0], parsed) < 0) { //execvp executes the command
            printf("\nCould not execute command...");
        }
        exit(0); //In case execvp fails
    } else { //Parent process, indicates the starting and stopping of process
        printf("--------------------Starting program --------------------------\n");
        wait(NULL); // Wait for the child to finish
        printf("__________________Program ended --------------\n");
        return;
    }
    attroff(COLOR_PAIR(2));
}

//Executes builtin commands
int cmdHandler(char** parsed) {
    int numCmds = 2, i, switchFlag = 0;
    char* commandList[numCmds];
    attron(COLOR_PAIR(2));
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
            printf("\nExiting Dino Shell\n");
            exit(0);
        case 2:
            chdir(parsed[1]);
            return 1;
        default:
            break;
    }
    return 0;
    attroff(COLOR_PAIR(2));
}

//Executes the pipe
void execArgsPiped(char** parsed, char** parsedPipe) {
    int pipe_fd[2];
    pid_t pid1, pid2; //Two procceses
    attron(COLOR_PAIR(2));

    if (pipe(pipe_fd) < 0) { //Creates a pipe via systemcall
        printf("\nPipe failed.\n");
        return;
    }

    pid1 = fork(); //First child fork
    if (pid1 < 0) {//In case the first fork fails
        printf("\nFork failed.\n");
        return;
    }

    if (pid1 == 0) {//Close the read end of pipe if its the first child
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);

        if (execvp(parsed[0], parsed) < 0) {//In case execution fails
            printf("\nExec failed.\n");
            exit(0);
        }
    }
        else {//If its the parent
            pid2 = fork();

            if (pid2 < 0) {//In case the second fork fails
                printf("\nFork failed.\n");
                return;
            }

            if (pid2 == 0) {//Closes the write end of the pipe
                close(pipe_fd[1]);
                dup2(pipe_fd[0], STDIN_FILENO);
                close(pipe_fd[0]);

                if (execvp(parsedPipe[0], parsedPipe) < 0) {//In case execution fails
                    printf("\nExec failed.\n");
                    exit(0);
                }
            }
            else {//Each wait is for a child
                wait(NULL);
                wait(NULL);
            }
        }
    attroff(COLOR_PAIR(2));
    }

//Finds then parses the pipe
int parsePipe(char* str, char** strpiped) {
    int i;
    attron(COLOR_PAIR(2));

    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, "|"); //Split string into substrings
        if (strpiped[i] == NULL) //When no more substrings need to be parsed
            break;
    }

        if (strpiped[1] == NULL) //If only one substring was found
            return 0;
        else {
            return 1;
        }
    attroff(COLOR_PAIR(2));
}

//Parses command words into two words
void parseSpace(char* str, char** parsed) {
    int i;

    for (i = 0; i < MAXLIST; i++) {
        parsed[i] = strsep(&str, " "); //Split the string into substrings

        if (parsed[i] == NULL) //When there are no more substrings
            break;

        if (strlen(parsed[i]) == 0) //When parsed substring is empty
            i--;
    }
}

//Proccess a given string and parsing it into arguments for execution
int processString(char* str, char** parsed, char** parsedPipe) {
    char* strpiped[2];
    int piped = 0;

    piped = parsePipe(str, strpiped); //Checks if the input string contains a pipe character

    if (piped) {
        parseSpace(strpiped[0], parsed); //No pipe was found
        parseSpace(strpiped[1], parsedPipe); //Pipe was found
    }

    else {
        parseSpace(str, parsed); //Parses the entire input string
    }

    if (cmdHandler(parsed)) //Calls cmdHandler to handle passed arguments
        return 0;

    else
        return 1 + piped;
}

int main(void) {
    //Curses libraries may not be needed
    initscr(); //Initializes the curses library
    start_color(); //Enables color support in the terminal
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // User color
    init_pair(2, COLOR_GREEN, COLOR_BLACK); //Program color
    motd();
    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char* parsedArgsPiped[MAXLIST];
    int execFlag = 0;

    while (1) {

        if (userInput(inputString)) 
            continue;
    
        execFlag = processString(inputString, parsedArgs, &parsedArgsPiped);
            //Calls the processString function to parse the user's input and determines

        if (execFlag == 1) //If the input is a simple command
            execArgs(parsedArgs);

        if (execFlag == 2) //If the input is a piped command
            execArgsPiped(parsedArgs, parsedArgsPiped);
    }
    endwin(); //Ends curses mode and restores the terminal to its orginal state
    return 0;
}
