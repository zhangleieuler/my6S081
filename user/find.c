#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

/**********************************************
* function   :
* param      :path: the char array the directory of the find path, target_file:the file name wanted,char array
* return     :void
* description:
* author     :leizhang
* date       :2024.10.12
* note       :find the file in the path,judge target; if find the directory, recures the directory node 
**********************************************/
void find(char* path, char* target_file)
{
    //buf:buffer， p:traversal pointer
    char buf[512], *p;
    int fd;
    struct dirent de;                   //file or subdirectory of current directory
    struct stat st;

    //open the directory specified by $path
    if((fd = open(path,O_RDONLY)) < 0)    
    {
        fprintf(2,"usage: find can not open %s\n",path);
        exit(1);
    }

    //获取目录的状态信息 get the status of the directory specified by fd
    if(fstat(fd,&st) < 0)
    {
        fprintf(2,"usage: find can not state %s\n",path);
        close(fd);
        exit(1);
    }
    //directory is file, error; is directory, recurse to find
    switch(st.type)
    {
        case T_FILE:
        fprintf(2,"usage: find dir file\n");
        exit(1);

        case T_DIR:
        //check whether building a full path will exceed the buffer size，first 1 is for '/', second 1 is for '\0'.DIRSIZ is the maximum size of the directory name at the next level,example: /home/user/document: path -> /home/user, 1 -> /, DIRSIZ -> document, 1 -> \0
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
        {
            printf("find: path too long\n");
            break;
        }
         //read the full path
        strcpy(buf,path);                          //copy string of path to buf
        p = buf + strlen(buf);                     //update the pointer to the last postion in the buffer
        *p++ = '/';                                 //append a '/' separator, buf == /home/user/
       
         //loop read the name of each file/folder under the folder specified specified by fd, which is $path,name is de.name  /home/user/document, document is the file or directroy in the current path 
        while(read(fd,&de,sizeof(de)) == sizeof(de))
        {
            //the inum of the struct dirent (directory) is 0, invalid entry, read the next file in current path. ignore the . and ..
            if((de.inum == 0) || (strcmp(de.name,".") == 0) || (strcmp(de.name,"..") == 0)) 
            {
                continue;      
            }
            //concatenate the full path name
            memmove(p,de.name,DIRSIZ);               
            p[DIRSIZ] = 0; 
            if(stat(buf,&st) < 0)                     //get the information of the file of full path 
            {
                fprintf(2,"usage: find cannot stat %s\n",buf);
                continue;                             //skip the current direactory, continue next.
            }
            // the opened  file is directory, recures the find function.
            if(st.type == T_DIR)
            {
                find(buf,target_file);
            }
            //opend file is file, judge if is target_file
            else if(st.type == T_FILE)
            {
                //check the file whether match the target_file
                if(strcmp(de.name,target_file) == 0)
                {
                    fprintf(1,"%s\n",buf);           
                }
            }
        }
       
        break;
    }
}


int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        fprintf(2,"usage: find you need pass in only 2 arguments\n");
        exit(1);
    }
    char* path = argv[1];
    char* target_file = argv[2];
    find(path,target_file);
    exit(0);
}
