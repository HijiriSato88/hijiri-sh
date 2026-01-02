#include "../shell.h"

static void print_permissions(mode_t mode) {
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
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

            struct stat st;
            if (stat(fullpath, &st) == -1) {
                perror(entry->d_name);
                continue;
            }

            print_permissions(st.st_mode);
            printf(" %3lu", (unsigned long)st.st_nlink);

            struct passwd *pw = getpwuid(st.st_uid);
            printf(" %-8s", pw ? pw->pw_name : "?");

            struct group *gr = getgrgid(st.st_gid);
            printf(" %-8s", gr ? gr->gr_name : "?");

            printf(" %8lld", (long long)st.st_size);

            char timebuf[64];
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));
            printf(" %s", timebuf);

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
