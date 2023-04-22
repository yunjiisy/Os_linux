#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
int main() {
    //배열의 첫번째 요소: 수행될 프로그램의 이름을 가리킴.
    //배열의 나머지 요소들은 프로그램에 대한 가능한 나머지 인수
    // ex) argv[0]= "echo";  argv[1]="hello world";
    // ex) argv[0] = "ls";  argv[1] = "-al";
    char *argv[3];
    argv[0] = "ls"; 
    //argv[1] = "-al";
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