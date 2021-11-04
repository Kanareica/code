#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>
#include <string.h>

int main(int argc, char **argv)
{
    int n, status, pid, cnt = 0;
    char str[PATH_MAX];
    sscanf(argv[1], "%d", &n);
    if (argc - 2 < n) {
        n = argc;
    }
    for (int i = 0; i < n; i++) {
        pid = fork();
        if (pid == -1) {
            _exit(1);
        } else if (!pid) {
            FILE *f = fopen(argv[i + 2], "r");
            fgets(str, PATH_MAX, f);
            str[strlen(str) - 1] = '\0';
            execlp(str, str, NULL);
            _exit(1);
        }
    }
    while (wait(&status) != -1) {
        cnt += (WIFEXITED(status) && !WEXITSTATUS(status));
    }
    for (int i = n + 2; i < argc; i++) {
        pid = fork();
        if (pid == -1) {
            _exit(1);
        } else if (!pid) {
            FILE *f = fopen(argv[i], "r");
            fgets(str, PATH_MAX, f);
            str[strlen(str) - 1] = '\0';
            execlp(str, str, NULL);
            _exit(1);
        }
        wait(&status);
        cnt += (WIFEXITED(status) && !WEXITSTATUS(status));
    }
    printf("%d\n", cnt);
    return 0;
}

