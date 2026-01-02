#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

void builtin_pwd(void) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd");
    }
}

void builtin_tty(void) {
    // isatty() で標準入力がターミナルに接続されているか確認
    if (!isatty(STDIN_FILENO)) {
        printf("not a tty\n");
        return;
    }

    char *tty_name = ttyname(STDIN_FILENO);
    if (tty_name != NULL) {
        printf("%s\n", tty_name);
    } else {
        perror("tty");
    }
}

void builtin_cd(char **argv) {
    const char *path;

    if (argv[1] == NULL) {
        path = getenv("HOME");
        if (path == NULL) {
            fprintf(stderr, "cd: HOME not set\n");
            return;
        }
    } else {
        path = argv[1];
    }

    if (chdir(path) != 0) {
        perror("cd");
    }
}

void execute_external(char **argv) {
    pid_t pid = fork();

    if (pid == 0) {
        execvp(argv[0], argv);
        // execvp が戻ってきたらエラー
        perror(argv[0]);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork");
    }
}

int parse_input(char *input, char **argv) {
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
        } else {
            // execute_external(argv);
            fprintf(stderr, "%s: command not found\n", argv[0]);
        }
    }

    return 0;
}
