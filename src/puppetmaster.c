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

    /* Print help if necessary */
    if (argc != 2)
    {
        fprintf(stderr, "USAGE: %s [executable]\n", argv[0]);
        return -1;
    }

    /* Create pipes for 2-way communication */
    pipe(p);
    pipe(c);

    /* Bail out if fork() fails */
    if ((pid = fork()) == -1)
    {
        perror("fork");
        return -2;
    }
    /* Enter here if we are the child pid */
    else if (pid == 0)
    {
        /* Duplicate file descriptors to overwrite
           stdin (0) and stdout (1) */
        dup2(c[0], 0);
        dup2(p[1], 1);

        /* Close the ends we dont need */
        close(c[1]);
        close(p[0]);

        /* Execute the command in place */
        execlp(argv[1], *(argv+1), NULL);
        perror("execve");
        return -3;
    }

    /* Continue here if we are the parent pid */
    /* Close the ends of the pipes we dont need */
    close(c[0]);
    close(p[1]);

    /* Create 4k buffer for reading child output */
    char *buffer = (char*)calloc(sizeof(char),4096);
    if (buffer == NULL)
    {
        perror("buffer");
        return -4;
    }

    /* Read input (output from child) */
    nbytes = read(p[0], buffer, 4095);
    if (nbytes > 0)
    {
        /* Display output from child */
        printf("captured buffer: [%s]\n", buffer);
    }

    return 0;
}
