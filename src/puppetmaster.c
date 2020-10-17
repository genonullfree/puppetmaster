#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    FILE *child;

    if (argc != 2)
    {
        fprintf(stderr, "USAGE: %s [executable]\n", argv[0]);
        return 1;
    }

    if (( child = popen(argv[1], "w")) == NULL)
    {
        perror("popen");
        return 1;
    }

    printf("hello world\n");

    pclose(child);

    return 0;
}
