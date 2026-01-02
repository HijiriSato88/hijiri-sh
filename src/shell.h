#ifndef SHELL_H
#define SHELL_H

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

// commands
void builtin_pwd(void);
void builtin_cd(char **argv);
void builtin_ls(char **argv);
void builtin_tty(void);

#endif
