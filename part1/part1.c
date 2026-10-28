#include <unistd.h>

/* Issues exactly five system calls beyond what an empty program does. */
int main(void)
{
    getpid();
    getppid();
    getuid();
    getgid();
    geteuid();
    return 0;
}
