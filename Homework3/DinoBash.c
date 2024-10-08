#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void motd() {
    printf("\n");
    printf("  Wecome to Dino Shell!\n");
    printf("-------------------------------\n");
    printf("Enter your commands below.\n");
    printf("Type 'exit' to quit.\n");
    printf("\n");
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
