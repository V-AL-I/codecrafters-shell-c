#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 100

void displayErrorNotFound(char input[SIZE]) {
    printf("%s: command not found\n", input);
}

int checkExit(char input[SIZE]) {
    // returns -1 if input is not exit
    // returns a positive int if input is exit, the int being the exit code found
    //
    if (strlen(input) == 6) {
        if (input[0] == 'e' && input[1] == 'x' && input[2] == 'i' && input[3] == 't') {
            return ((int) input[5]) - 48;
        }
    }
    return -1;

}

int main() {
    // You can use print statements as follows for debugging, they'll be visible when running tests
    //printf("Logs from your program will appear here!\n");

    //Uncomment this block to pass the first stage
    
    // Wait for user input
    char input[SIZE];
    while(1) {
        printf("$ ");
        fflush(stdout);

        fgets(input, SIZE, stdin);
        input[strlen(input) - 1] = '\0';
        int exit = checkExit(input);
        if (exit >= 0) {
            return exit;
        }
        displayErrorNotFound(input);
    }
    
    return 0;
}
