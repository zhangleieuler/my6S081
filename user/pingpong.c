#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


#define READEND 0
#define WRITEEND 1


int
main(int argc, char* argv[])
{
    //pipe is half duplex,create two pipes to make parent and child process can write and read
    //pipe is half-duplex
    int p1[2];
    int p2[2];
    pipe(p1);                         //p1:parent write, child red
    pipe(p2);                         //p2:parent read,  child write
    
    
    int pid;
    pid = fork();

    if(0 > pid)
    {
        fprintf(2,"usage: pingpong fork failed\n");
        exit(1);
    }
    else if(0 == pid)               //child process
    {
        char buf2[] = "pong";
        char str2[50] = {0};
        close(p1[WRITEEND]);        //child close write end of p1,so p1 read end choke in child.
        close(p2[READEND]);         
        read(p1[READEND],str2,sizeof(str2));
        printf("%d: received %s\n",getpid(),str2);
        write(p2[WRITEEND],buf2,strlen(buf2));
        close(p1[READEND]);       
        close(p2[WRITEEND]);
        exit(0);
    }
    else                            //parent process
    {
        char buf1[] = "ping";
        char str1[50] = {0};
        close(p1[READEND]);         
        close(p2[WRITEEND]);        //parent close write of p2,so p2 read end choke in parent
        write(p1[WRITEEND],buf1,strlen(buf1));
        read(p2[READEND],str1,sizeof(str1));
        printf("%d: received %s\n",getpid(),str1);
        close(p1[WRITEEND]);
        close(p2[READEND]);
        exit(0);
    }
}