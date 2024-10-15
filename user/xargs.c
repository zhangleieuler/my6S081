#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/fcntl.h"
/**********************************************
* file       :
* description:
* author     :leizhang
* date       :
* note       :
**********************************************/
/*
input:

output:


input:

output:

*/
#define MAXBUFLEN 1024

int readline(char* new_argv[], int curr_argc)
{
    char buf[MAXBUFLEN];          //用于存储输入行的缓冲区
    int n = 0;                    //读取的字符的数量

    //从标准输入读取字符，直到遇到换行符或这达到缓冲区最大长度
    while(read(0,buf+n,1) != 0)
    {
        if(n >= (MAXBUFLEN-1))    //减1是因为要预留一个字节给结束字符0
        {
            fprintf(2,"Usage:xargs readline argument is too long\n");
            exit(1);
        }
        //遇到换行符，停止读取;在主函数中再次调用readline函数进行读取
        if(buf[n] == '\n')
        {
            break;
        }
        n++;                      //增加读取字符计数
    }
    //对读取的行进行处理
    buf[n] = 0;                   //最末尾为结束字符0
    if(0 == n)                    //没有读取到任何字符，返回0
    {
        return 0;
    }
    int offset = 0;               //用于遍历缓冲区的偏移量
    //将输入的行分割成参数
    while(offset < n)             //偏移量小于读取到的字符
    {
        //将新读取的字符的地址放入参数列表curr_argc位置处，然后再更新curr_argc
        new_argv[curr_argc++] = buf+offset;
        //继续遍历，知道遇到空格
        while(buf[offset] != ' ' && offset < n)
        {
            offset++;
        }
        //遇到空格，将空格替换为字符串终止符，实现字符分隔
        while(buf[offset] == ' ' && offset < n)
        {
            buf[offset++] = 0;
        }
    }
    return curr_argc;             //返回新参数列表的总数
}


int
main(int argc, char * argv[])
{
    //argv has no element, argc == 1, only have command and no arguments
    //argv[0]为xargs，在这里为告诉xv6 shell, 要运行xargs程序
    if(argc <= 1)
    {
        fprintf(2,"Usage: xargs commans (arg ...) error\n");
        exit(1);
    }
    
    char* command = malloc(strlen(argv[1]) + 1); //+1的原因是保留一个结束位为0字符
    //开辟内存失败检查
    if(0 == command)
    {
        fprintf(2,"Usage: xargs command memory allocation failed\n");
        exit(1);
    }
    //将命令复制到变量(variable)command中
    strcpy(command,argv[1]);

    char* new_argv[MAXARG];             //新参数列表，最后存储的是command 后面的所有参数
    //将xargs 命令 参数列表中的参数全部读取
    for(int i = 1; i < argc; ++i)
    {
        new_argv[i-1] = malloc(strlen(argv[i]) + 1);
        //new_argc[0]为command
        strcpy(new_argv[i-1],argv[i]);
    }
    //此时new_argv数组的实际规模为argc-1


    //将 xargs 前面的参数列表读取，并且放入new_argv数组中
    int curr_argc;      //当前参数数量
    //循环读取输入，直到没有更多的输入,当没有输入时，即curr_arc = 0
    //第一个实参为新参数的数组，第二个实参为参数的新参数数组一开始的个数
    while((curr_argc = readline(new_argv,argc-1)) != 0)
    {
        new_argv[curr_argc] = 0;       //结束参数列表
        //子进程用exec来执行命令
        if(0 == fork())
        {
            exec(command,new_argv);    //在xargs程序中，执行命令
            exit(0);                   
        }
        wait(0);                       //父进程等待子进程结束，并回收子进程
    }

    //释放分配的内存  free the memory by malloc
    free(command);                    //释放命令内存
    for(int i = 0; i < argc -1; ++i)
    {
        free(new_argv[i]);               //释放参数内存
    }
    exit(0);            //why exit(0)and not return 0?
}