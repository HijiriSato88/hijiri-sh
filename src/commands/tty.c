#include "../shell.h"

void builtin_tty(void) {
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
