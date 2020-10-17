#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    FILE *child;
    int pid;
    int p[2];   /* parent: r, w -- parent reads from child */
    int c[2];   /*  child: r, w -- child reads from parent*/
    int nbytes;

    if (argc != 2)
    {
        fprintf(stderr, "USAGE: %s [executable]\n", argv[0]);
        return 1;
    }

    pipe(p);
    pipe(c);

    if (fork() == -1)
    {
        perror("fork");
        return 1;
    }
    else if (pid == 0) /* child */
    {
        dup2(0, p[0]);
        dup2(1, c[1]);

        return 0;
    }

    /* parent */
    dup2(1, p[1]);
    dup2(0, c[0]);

    return 0;
}
