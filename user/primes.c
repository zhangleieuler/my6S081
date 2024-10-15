#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
/**********************************
 * file       : primes.c
 * descripotor: concurrent version of prime sieve using pipes
 * author     : zhanglei 
 * date       : 2024.10.05
 * note        :素数筛法
 **********************************/

#define READEND  0
#define WRITEEND 1

/*********************************
 * function :
 * param    :
 * author   :        date:
 * note     :
 */

void new_proc(int p[2])
{
    int prime;
    int flag;  
    int next;                          //second element in pipe buffer
    close(p[WRITEEND]);                //as child, close the write end
    //get the first integral as the current prime
    if(read(p[READEND], &prime, 4) != 4)
    {
        fprintf(2,"usage: new_proc read pipe error\n");
        exit(1);
    }
    //the first element in pipe buffer is prime
    // printf("process: %d sieve the prime %d\n",getpid(),prime);       //find the prime belong to the process
    printf("prime %d\n",prime);       //find the prime belong to the process
    //read the next integral
    flag  = read(p[READEND],&next, 4);
    if(flag)
    {
        int newp[2];                  //create new pipe
        pipe(newp);
        int pid = fork();
        if(0 == pid)
        {
            new_proc(newp);          //recursion
        }
        else if(0 < pid)
        {
            close(newp[READEND]); 
            if(next % prime)         //next is not times of  the current prime
            {
                //new prime write the next element to the new pipe write end
                write(newp[WRITEEND], &next, 4);  
            }
            //continue to read integral in pipe to sieve the times of the current prime
            while(read(p[READEND], &next, 4))
            {
                if(next % prime)
                {
                    write(newp[WRITEEND], &next, 4);
                }
            }
            close(p[READEND]);
            close(newp[WRITEEND]);
            wait(0);
        }
    }
    exit(0);
}

int 
main()
{
    int p[2];
    int ret = pipe(p);                  //pipe         
    if(0 != ret)
    {
        fprintf(2,"usage: primes pipe error\n");
        exit(1);
    }
    int pid = fork();
    if(0 > pid)
    {
        fprintf(2,"usage: primes fork error\n");
        exit(1);
    }
    //child's recursion function, transfer data in pipe to new_proc func
    if(0 == pid)
    {
        new_proc(p);         
    }
    //parent
    else if(0 < pid)        
    {
        close(p[READEND]);  //parent close read end of pipe
        for(int i = 2; i <= 35; i++)
        {
            //parent write number to write end of pipe, 4 Bytes
            if(write(p[WRITEEND], &i, 4) != 4)
            {
                fprintf(2,"usage: primes first process write%d to pipe error\n", i);
                exit(1);
            }
        }
        close(p[WRITEEND]);
        wait(0);            //first parent process wait child process exit
        exit(0);
    }
    return 0;
}