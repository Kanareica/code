#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/wait.h>

volatile int fl = 0;

void
handler(int sig)
{
    signal(SIGUSR1, handler);
    fl = 0;
}

int main(int argc, char **argv)
{
    int pid_1, pid_2, val = 1, n, fd[2];
    pipe(fd);
    signal(SIGUSR1, handler);
    sscanf(argv[1], "%d", &n);
    pid_1 = fork();
    if (!pid_1) {
        while (1) {
            while (!fl){
                pause();
                read(fd[0], &val, sizeof(val));
                if (val == 1) {
                    read(fd[0], &pid_2, sizeof(pid_2));
                }
                if (val >= n) {
                    close(fd[1]);
                    close(fd[0]);
                    raise(SIGTERM);
                }
                fl = 0;
                printf("%d %d\n", 1, val);
                fflush(stdout);
                val++;
                write(fd[1], &val, sizeof(val));
                kill(pid_2, SIGUSR1);
            }
        }
    }
    
    pid_2 = fork();
    if (!pid_2) {
        while (1) {
            while (!fl){
                pause();
                read(fd[0], &val, sizeof(val));
                if (val >= n) {
                    close(fd[1]);
                    close(fd[0]);
                    raise(SIGTERM);
                }
                fl = 0;
                printf("%d %d\n", 2, val);
                fflush(stdout);
                val++;
                write(fd[1], &val, sizeof(val));
                kill(pid_1, SIGUSR1);
            }
        }
    }
    printf("Done\n");
    write(fd[1], &val, sizeof(val));
    write(fd[1], &pid_2, sizeof(pid_2));
    kill(pid_1, SIGUSR1);
    close(fd[0]);
    close(fd[1]);
    int pid = wait(NULL);
    if (pid == pid_1) {
        kill(pid_2, SIGTERM);
    } else {
        kill(pid_1, SIGTERM);
    }
    printf("Done\n");
    fflush(stdout);
    return 0;
}

