#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

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

void print_permissions(mode_t mode) {
    // ファイルタイプ
    char type = '-';
    if (S_ISDIR(mode)) type = 'd';
    else if (S_ISLNK(mode)) type = 'l';

    printf("%c", type);
    printf("%c", (mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (mode & S_IXUSR) ? 'x' : '-');
    printf("%c", (mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (mode & S_IXGRP) ? 'x' : '-');
    printf("%c", (mode & S_IROTH) ? 'r' : '-');
    printf("%c", (mode & S_IWOTH) ? 'w' : '-');
    printf("%c", (mode & S_IXOTH) ? 'x' : '-');
}

void builtin_ls(char **argv) {
    const char *path = ".";
    int long_format = 0;

    // オプションと引数の解析
    for (int i = 1; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            long_format = 1;
        } else {
            path = argv[i];
        }
    }

    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("ls");
        return;
    }

    struct dirent *entry;
    int first = 1;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }

        if (long_format) {
            // フルパスを構築
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

            struct stat st;
            if (stat(fullpath, &st) == -1) {
                perror(entry->d_name);
                continue;
            }

            // パーミッション
            print_permissions(st.st_mode);

            // リンク数
            printf(" %3lu", (unsigned long)st.st_nlink);

            // 所有者
            struct passwd *pw = getpwuid(st.st_uid);
            printf(" %-8s", pw ? pw->pw_name : "?");

            // グループ
            struct group *gr = getgrgid(st.st_gid);
            printf(" %-8s", gr ? gr->gr_name : "?");

            // サイズ
            printf(" %8lld", (long long)st.st_size);

            // 更新日時
            char timebuf[64];
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));
            printf(" %s", timebuf);

            // ファイル名
            printf(" %s\n", entry->d_name);
        } else {
            if (!first) {
                printf(" ");
            }
            printf("%s", entry->d_name);
            first = 0;
        }
    }

    if (!long_format) {
        printf("\n");
    }

    closedir(dir);
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
        } else if (strcmp(argv[0], "ls") == 0) {
            builtin_ls(argv);
        } else {
            // execute_external(argv);
            fprintf(stderr, "%s: command not found\n", argv[0]);
        }
    }

    return 0;
}
