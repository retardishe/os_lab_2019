#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>

int main() {
    pid_t child_pid = fork();
    if (child_pid > 0)
            execl("sequential_min_max", "sequential_min_max", "5", "4", NULL);
    if (child_pid == 0) {
            printf("Fork: successfully\n");
    }
    else
        exit(0);
}