#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(){
    int pid = fork();

    if (pid > 0) {
        printf("parent: child=%d\n",pid);
        pid = wait(NULL);
        printf("child %d is done\n", pid);
    }
    else if (pid == 0){
        printf("child: existing\n");
    }
    else {
        printf("fork error\n");
    }
    return 0;
}
