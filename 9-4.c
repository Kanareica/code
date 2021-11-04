#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>

extern char **environ;

int 
main(int argc, char **argv)
{
    if (argc == 1) {
        return 0;
    }
    char path[PATH_MAX], line[PATH_MAX * 2 + 8];
    const char *set = "/tmp";
    int fd, i;
    const char *tmp = getenv("XDG_RUNTIME_DIR");
    if (!tmp) {
        tmp = getenv("TMPDIR");
        if (!tmp) {
            tmp = set;
        }
    }
    snprintf(path, PATH_MAX, "%s/run_%d.py", tmp, getpid());
    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0700);
    FILE *f = fdopen(fd, "w");
    fprintf(f, "#!/usr/bin/env python\nprint(");
    for (i = 1; i < argc - 1; i++) {
        fprintf(f, "%s*", argv[i]);
    }
    fprintf(f, "%s)", argv[i]);
    fclose(f);
    snprintf(line, PATH_MAX * 2 + 8, "%s && rm %s", path, path);
    execl("/bin/sh", "sh", "-c", line, NULL);
    exit(0);
}
