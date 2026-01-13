#include "shell.h"

static int parse_input(char *input, char **argv) {
    int argc = 0;
    char *token = strtok(input, " \t");

    while (token != NULL && argc < MAX_ARGS - 1) {
        argv[argc++] = token;
        token = strtok(NULL, " \t");
    }
    argv[argc] = NULL;

    return argc;
}

int main(void) {
    char input[MAX_INPUT];
    char *argv[MAX_ARGS];

    while (1) {
        printf("hijiri-sh> ");
        fflush(stdout);

        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0) {
            continue;
        }

        int argc = parse_input(input, argv);
        if (argc == 0) {
            continue;
        }

        if (strcmp(argv[0], "exit") == 0) {
            break;
        } else if (strcmp(argv[0], "pwd") == 0) {
            builtin_pwd();
        } else if (strcmp(argv[0], "cd") == 0) {
            builtin_cd(argv);
        } else if (strcmp(argv[0], "tty") == 0) {
            builtin_tty();
        } else if (strcmp(argv[0], "ls") == 0) {
            builtin_ls(argv);
        } else if (strcmp(argv[0], "cat") == 0) {
            builtin_cat(argv);
        } else {
            fprintf(stderr, "%s: command not found\n", argv[0]);
        }
    }

    return 0;
}
