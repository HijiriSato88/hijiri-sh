#include "../shell.h"

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
