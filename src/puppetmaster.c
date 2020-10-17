#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    int pid;
    int p[2];   /* parent: r, w -- parent reads from child */
    int c[2];   /*  child: r, w -- child reads from parent*/
    int nbytes;

    if (argc != 2)
    {
        fprintf(stderr, "USAGE: %s [executable]\n", argv[0]);
        return -1;
    }

    pipe(p);
    pipe(c);

    if ((pid = fork()) == -1)
    {
        perror("fork");
        return -2;
    }
    else if (pid == 0) /* child */
    {
        dup2(c[0], 0);
        dup2(p[1], 1);

        close(c[1]);
        close(p[0]);

        execlp(argv[1], argv[1], NULL);
        perror("execve");
        return -3;
    }

    /* parent */
    dup2(c[1], 1);
    dup2(p[0], 0);

    close(c[0]);
    close(p[1]);

    char *buffer = (char*)calloc(sizeof(char),4096);
    if (buffer == NULL)
    {
        perror("buffer");
        return -4;
    }

    while ((nbytes = read(p[0], buffer, 4095)) != 0)
    //if (nbytes > 0)
    {
        printf("captured buffer: [%s]\n", buffer);
    }

    return 0;
}
