// C libraries
#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#include<sys/types.h>

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

int userInput(char *str);
void execArgs(char **parsed);
int cmdHandler(char **parsed);
int cd(char *path) {
    if (cd(path) != 0) {
        perror("chdir"); //Prints and error message
        return 1;
    }
    return 0;
}
int main(void) {
    motd();

    while (1) {
        printf("dino-shell$ "); // Start of the prompt
        char input[256]; // User input can be up to 256 characters long
        fgets(input,sizeof(input), stdin); // Reads a line from specified stream and stores it into the string
            // Memory of string, size of string, stream

        // Exit condition
        if (strcmp(input, "exit\n") == 0) { // strcmp is used to compare strings, comparing the input and 'exit'
            printf("Exiting Dino Shell.\n");
            break;
        }
    }
    return 0;
}
