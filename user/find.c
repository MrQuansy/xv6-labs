#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char * path,char *file)
{
    int fd;
    struct dirent de;
    struct stat st;
    char buf[512], *p;

    if((fd = open(path, 0)) < 0){//打开目录，获得对应的文件描述符
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    while(read(fd, &de, sizeof(de)) == sizeof(de))//使用while循环得到目录下所有文件/子目录的dirent结构体de
    {
        if(strcmp(de.name,".")==0||strcmp(de.name,"..")==0)
            continue;//跳过

        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        char *pp=de.name;
        while(*pp!=0) //字符串拼接
        {
            *p++ =*pp++;
        }
        *p=0;//别忘了加终止符

        if(stat(buf, &st) < 0)//获取buf的st
        {
            printf("find: cannot stat %s\n", buf);
            continue;
        }
        if(st.type==T_FILE)//如果buf的st类型是文件类型
        {
            if(strcmp(de.name,file)==0)//如果文件名字和我们需要搜索的文件名字相同，那么就直接输出文件信息
            {
                printf("%s\n",buf);
            }
        }
        else if(st.type==T_DIR)//如果buf的st类型是目录类型
        {
            if (de.inum == 0) //目录下没有文件，如果缺少这个语句xargs命令将会报错
                continue;
            find(buf,file);//递归，继续搜索
        }

    }
}

int main(int argc, char *argv[])
{
    char * path=argv[1];//查找目录
    char * file=argv[2];//查找文件

    find(path,file);

    exit(0);
}
