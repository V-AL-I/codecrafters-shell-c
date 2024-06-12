#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 500
#define NB_OF_BUILTIN 3

typedef struct {
    char* name;
    int size;
} Builtin;


void displayErrorNotFound(char input[SIZE]) {
    printf("%s: command not found\n", input);
}

int checkExit(char input[SIZE]) {
    // returns -1 if input is not exit
    // returns a positive int if input is exit, the int being the exit code displayErrorNotFound
    if (strlen(input) == 6) {
        if (strncmp(input, "exit", 4) == 0) {
            return ((int) input[5]) - 48;
        }
    }
    return -1;

}

int checkEcho(char input[SIZE]) {
    return strncmp(input, "echo", 4) == 0;
}

int checkType(char input[SIZE]) {
    return strncmp(input, "type", 4) == 0;
}

int checkPwd(char input[SIZE]) {
    return strncmp(input, "pwd", 3) == 0;
}

int checkCd(char input[SIZE]) {
    return strncmp(input, "cd", 2) == 0;
}

void pwd() {
    char wd[SIZE];
    getcwd(wd, SIZE);
    printf("%s\n", wd);
}

void echo(char input[SIZE]) { 
    input += 5;
    printf("%s\n", input);
}

char** getPaths(char* path, int* path_count) {
    path_count[0] = 0;
    for (int i = 0; path[i]; i++) {
        if (path[i] == ':') path_count[0]++;
    }
    path_count[0]++;
    char** filepaths = calloc(path_count[0], sizeof(char*));
    for (int i = 0; i < path_count[0]; i++) filepaths[i] = calloc(SIZE, sizeof(char));
    int x = 0;
    int y = 0;
    for (int i = 0; path[i]; i++) {
        if (path[i] != ':') {
            filepaths[x][y++] = path[i];
        }
        else {
            //printf("%s\n", filepaths[x]);
            filepaths[x++][y] = '\0';
            y = 0;
        }
    }
    //printf("getPaths was successfull\n");
    return filepaths;
}



void type(char input[SIZE], Builtin builtin[NB_OF_BUILTIN]) {
    input += 5;
    for (int i = 0; i < NB_OF_BUILTIN; i++) {
        if (strncmp(builtin[i].name, input, builtin[i].size) == 0) {
            printf("%s is a shell builtin\n", input);
            return;
        }
    }
    char* PATH = getenv("PATH");
    //printf("%s\n", PATH);

    int* path_count = calloc(1, sizeof(int));
    char** filepaths = getPaths(PATH, path_count);
    //printf("path_count = %i\n", path_count[0]);
    for (int i = 0; i < path_count[0]; i++) {
        char fullpath[strlen(filepaths[i]) + strlen(input)];
        sprintf(fullpath, "%s/%s", filepaths[i], input);
        //printf("%s\n", fullpath);
        if (access(fullpath, X_OK) == 0) {
            printf("%s is %s\n", input, fullpath);
            return;
        }
    }
    printf("%s not found\n", input);
}

void execute(char input[SIZE]) {
    char* PATH = getenv("PATH");
    int* path_count = calloc(1, sizeof(int));
    char** filepaths = getPaths(PATH, path_count);

    for (int i = 0; i < path_count[0]; i++) {
        char* inputCopy = calloc(SIZE, sizeof(char));
        strcpy(inputCopy, input);
        char* command = strtok(inputCopy, " ");
        char fullpath[strlen(filepaths[i]) + strlen(command)];
        sprintf(fullpath, "%s/%s", filepaths[i], command);
        if (access(fullpath, X_OK) == 0) {
            char exec[strlen(filepaths[i]) + strlen(input)];
            sprintf(exec, "%s/%s", filepaths[i], input);
            //printf("%s\n", input);
            system(exec);
            return;
        }
    }
    displayErrorNotFound(input);
}

void cd(char input[SIZE]) {
    input += 3;
    char* inputCopy = calloc(strlen(input), sizeof(char));
    strcpy(inputCopy, input);

    if (strncmp(input, "./", 2) == 0) {
        input++;
        char newDir[SIZE];
        char cwd[SIZE];
        getcwd(cwd, SIZE);
        sprintf(newDir, "%s%s", cwd, input);
        printf("%s\n", newDir);
        if (chdir(newDir)) printf("cd: %s: No such file or directory\n", inputCopy);
    }
    else if (strncmp(input, "000", 3) == 0) {
        input+=3;
        int backCount = 1;
        while(strncmp(input, "../", 3) == 0) {
            input += 3;
            backCount++;
        }
        char cwd[SIZE];
        getcwd(cwd, SIZE);
        int i = SIZE - 1;
        int count = 0;
        for (; cwd[i]; i--) {
            if (cwd[i] == '/') {
                count++;
                if (count == backCount) break;
            } 
        }
        char* newDir = calloc(i+1, sizeof(char));
        int j = 0;
        for (; j < i; j++) {
            newDir[j] = cwd[j];
        }
        newDir[j] = '\0';

        printf("newDir = %s\n", newDir);
        if (chdir(newDir)) printf("cd: %s: No such file or directory\n", inputCopy);

    }
    else if (chdir(input)) {
        printf("cd: %s: No such file or directory\n", input);
    }
}

int main() {
    Builtin myType = {"type", 4};
    Builtin myEcho = {"echo", 4};
    Builtin myExit = {"exit", 4};
    Builtin myPwd = {"pwd", 3};
    Builtin myCd = {"cd", 2};

    Builtin builtin[NB_OF_BUILTIN] = {myType, myEcho, myExit};
    
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
        else if (checkEcho(input)) {
            echo(input);
        }
        else if (checkType(input)) {
            type(input, builtin);
        }
        else if (checkPwd(input)) {
            pwd();
        }
        else if (checkCd(input)) {
            cd(input);
        }
        else {
            execute(input);
        }
    }
    return 0;
}
