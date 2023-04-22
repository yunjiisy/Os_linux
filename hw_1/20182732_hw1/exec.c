#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    char *argv[3];
    argv[0] = "ls";
    argv[1] = NULL;
    
    int pid = fork();

     if (pid > 0) {
        printf("parent: child=%d\n",pid);
        pid = wait(NULL);
        printf("child %d is done\n", pid);
    }
    else if (pid == 0){
        printf("child: existing\n");

        execve("/bin/ls", argv, NULL);
    }
    else {
        printf("fork error\n");
    }


}
