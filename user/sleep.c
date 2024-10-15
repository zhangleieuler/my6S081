#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int 
main(int argc, char* argv[])
{
    if(argc <= 1)
    {
        fprintf(2,"ERROR: sleep time required\n");
        exit(1);
    }
    int time = atoi(argv[1]);          //argv[1]为睡眠时间字符串，atoi(char*)将字符串转换为数字
    sleep(time);
    fprintf(2,"xv6 is sleeping...\n");
    exit(0);
}