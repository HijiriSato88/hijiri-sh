#include "../shell.h"

#define BUF_SIZE 4096

static int cat_file(int fd) {
    char buf[BUF_SIZE];
    ssize_t n;

    while ((n = read(fd, buf, BUF_SIZE)) > 0) {
        if (write(STDOUT_FILENO, buf, n) != n) {
            perror("cat: write error");
            return -1;
        }
    }

    if (n < 0) {
        perror("cat: read error");
        return -1;
    }

    return 0;
}

void builtin_cat(char **argv) {
    // 引数がなければ標準入力から読む
    if (argv[1] == NULL) {
        cat_file(STDIN_FILENO);
        return;
    }

    // 各ファイルを順番に処理
    for (int i = 1; argv[i] != NULL; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            perror(argv[i]);
            continue;
        }

        cat_file(fd);
        close(fd);
    }
}
