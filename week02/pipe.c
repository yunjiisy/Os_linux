#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int p[2]; 
    char* argv[2];

    argv[0] = "wc";
    argv[1] = NULL;

    pipe(p); // p[0]: readfd, p[1]: writefd 
    if(fork() == 0) {
        close(0);
        dup(p[0]);
        close(p[0]);
        close(p[1]); // close unused write end 
        execve("/bin/wc", argv, NULL);
    } 
    else {
        close(p[0]); // close unused read end 
        write(p[1], "hello world\n", 12); 
        close(p[1]);
    } 
}
