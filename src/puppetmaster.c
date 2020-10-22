#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>

//TODO: Make this cleaner
uint8_t _code[][5] = {
    {0x41, 0x42, 0x43, 0x0a, 0x00},
    {0x44, 0x45, 0x46, 0x0a, 0x00},
    {0x47, 0x48, 0x49, 0x0a, 0x00},
    0x00
};

int main(int argc, char *argv[])
{
    int pid = 0;
    int p[2];   /* parent: r, w -- parent reads from child */
    int c[2];   /*  child: r, w -- child reads from parent*/
    int nbytes   = 0;
    int count    = 0;
    int flags    = 0;
    char *buffer = NULL;

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

    /* Set the reading pipe to be non-blocking */
    flags = fcntl(p[0], F_GETFL, 0);
    fcntl(p[0], F_SETFL, flags | O_NONBLOCK);

    /* Create 4k buffer for reading child output */
    buffer = (char*)calloc(sizeof(char),4096);
    if (buffer == NULL)
    {
        perror("buffer");
        return -4;
    }

    /* Read input (output from child) */
    if ((nbytes = read(p[0], buffer, 4095)) > 0)
    {
        /* Display output from child */
        printf("captured buffer, len(%d): [%s]\n", nbytes, buffer);
    }

    while (_code[count][0] != 0x00)
    {
        /* Write to child pid's input */
        if (write(c[1], _code[count], 5) < 1)
        {
            /* Break out if error */
            perror("write");
            break;
        }

        usleep(1500);

        /* Read input (output from child) */
        if ((nbytes = read(p[0], buffer, 4095)) > 0)
        {
            /* Display output from child */
            printf("captured buffer len(%d): [%s]\n", nbytes, buffer);
        }

        count++;
    }

    return 0;
}
