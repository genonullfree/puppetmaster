#include <stdio.h>
#include <stdlib.h>
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

    if ((pid = fork()) == -1)
    {
        perror("fork");
        return 1;
    }
    else if (pid == 0) /* child */
    {
        dup2(0, c[0]);
        dup2(1, p[1]);

        execve(argv[1], argv+1, NULL);
        perror("execve");
        return 1;
    }

    /* parent */
    dup2(1, c[1]);
    dup2(0, p[0]);

    char *buffer = (char*)calloc(sizeof(char),4096);
    if (buffer == NULL)
    {
        perror("buffer");
        return 1;
    }

    nbytes = read(p[0], buffer, 4095);
    if (nbytes > 0)
    {
        printf("captured buffer: [%s]\n", buffer);
    }

    return 0;
}
