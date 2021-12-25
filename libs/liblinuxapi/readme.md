# Readme.md


记录测试代码

#### 目录操作

```c
#include <dirent.h>

void dir(void)
{

    DIR *dp;
    struct dirent *dirp;
    char *dir = "test_dir";

    system("mkdir test_dir");
    system("mkdir test_dir/a");
    system("mkdir test_dir/b");
    system("mkdir test_dir/c");

    /* 读目录 */
    if ((dp = opendir(dir)) == NULL)
    {
        fprintf(stderr, "Can't open %s", dir);
    }
    else
    {
        printf("索引节点号\t");
        printf("偏移量\t");
        printf("文件名长度\t");
        printf("文件名\t");
        printf("文件类型\n");
        while ((dirp = readdir(dp)) != NULL)
        {
            printf("%ld\t", dirp->d_ino);
            printf("%ld\t", dirp->d_off);
            printf("%d\t", dirp->d_reclen);
            printf("%s\t", dirp->d_name);
            printf("%d\n", dirp->d_type);
        }
    }

    closedir(dp);
    system("rm -r test_dir");
}
```

#### 文件操作

Linux有很多函数会有一个带有at后缀的函数，例如`open`和`openat`，区别是open默认通过AT_FDCWD宏来设置以当前路径作为相对路径的起点，`openat`允许用户传入一个文件描述符来设置相对路径的起点。
文件操作中，write、read都是从offset递增的读写，偏移值通过lseek设置。

在打开文件时设置O_APPEND标志来实现多个进程写同一文件时的原子操作


```c

void test(void)
{
    char *dir = "file";

    if (creat(dir, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        char buf1[20] = "123455666789";
        char buf2[20] = "abcdefghijk";
        if (write(fd, buf1, sizeof(buf1)) != sizeof(buf1))
            fprintf(stderr, "lseek error:%s\n", strerror(errno));

        off_t off = lseek(fd, -sizeof(buf1), SEEK_CUR);
        if (off == -1)  // Don't test it is less than 0
            fprintf(stderr, "lseek error:%s\n", strerror(errno));

        if (read(fd, buf2, sizeof(buf2)) < 0)
            fprintf(stderr, "lseek error:%s\n", strerror(errno));

        printf("read str:%s\n", buf2);

        off = lseek(fd, 10, SEEK_CUR);
        if (off == -1)  // Don't test it is less than 0
            fprintf(stderr, "lseek error:%s\n", strerror(errno));

        if (write(fd, buf2, sizeof(buf2)) != sizeof(buf2))
            fprintf(stderr, "lseek error:%s\n", strerror(errno));

        // 使用命令读文件实际内容`od xxx`，显示字符`od -c xxx`,显示十六进制`od -h xxx`

        close(fd);
        fd = -1;
    }
}
```

**多进程/线程时文件操作需要添加原子操作的情况：**
1. 多进程时向文件末尾数据，会出现使用lseek时会误判了当前文件末尾的位置，导致某个进程写的数据丢失。解决方法：每次写数据前调用lseek设置偏移量为文件末尾，或者在调用open时添加O_APPEND让每次写数据都在文件末尾写入。
2. 多进程时读写文件的某个位置时，出现读/写冲突。解决方法：使用fwrite、fread来在指定偏移位置读写数据
   
```c
void test(void)
{
    char *dir = "file";

    if (creat(dir, 0777) == -1)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        char buf1[20] = "123455666789";
        char buf2[20] = "abcdefghijk";
        if (pwrite(fd, buf1, sizeof(buf1), 25) != sizeof(buf1))
            fprintf(stderr, "lseek error:%s\n", strerror(errno));

        if (pwrite(fd, buf2, sizeof(buf2), 10) != sizeof(buf2))
            fprintf(stderr, "lseek error:%s\n", strerror(errno));

        if (pread(fd, buf1, sizeof(buf1), 25) < 0)
            fprintf(stderr, "lseek error:%s\n", strerror(errno));
        printf("pread res:%s\n", buf1);

        if (pread(fd, buf1, sizeof(buf1), 10) < 0)
            fprintf(stderr, "lseek error:%s\n", strerror(errno));
        printf("pread res:%s\n", buf1);

        close(fd);
        fd = -1;
    }
}
```

3. 多进程创建文件时，会出现文件已存在导致创建失败。解决方法：创建文件前使用open来检查该文件是否存在；或者直接在调用open时添加O_CREAT参数

**使用dup复制文件描述符，用来给多个进程共享同一个文件资源**

```c
#include <unistd.h>
void test(void)
{
    char *dir = "file";

    if (creat(dir, 0777) == -1)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        int fd_new = dup(fd);
        printf("fd:%d  fd_new:%d\n", fd, fd_new);

        int fd_new2 = dup2(fd, 20);         // 会关闭原来fd=20的文件
        printf("fd:%d  fd_new2:%d\n", fd, fd_new2);

        int fd_new3 = dup2(fd, fd);
        printf("fd:%d  fd_new2:%d\n", fd, fd_new3);

        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
        if (close(fd_new))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
        if (close(fd_new2))
            fprintf(stderr, "Close error:%s\n", strerror(errno));

        /* There will return error, because fd_new3=fd had closed */
        if (close(fd_new3))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}
```

**文件数据的同步**

```c
#include <unistd.h>
void test(void)
{
    char *dir = "file";

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        sync();         // 将块缓冲写入队列，但不保证写入磁盘
        fsync(fd);      // 将文件的数据写入磁盘，并同步更新文件属性
        fdatasync(fd);  // 将文件写入的数据写入磁盘

        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}
```
**修改已打开的文件属性**

通过fcntl函数来修改以及打开的文件属性，共有5种功能：
1. 复制已有的描述符(F_DUPFD/F_DUPFD_CLOEXEC)
2. 获取/设置文件描述符标志(F_GETFD/F_SETFD)
3. 获取/设置文件状态标志(F_GETFL/F_SETFL)
4. 获取/设置异步I/O所有权(F_GETOWN/F_SETOWN)
5. 获取/设置记录锁(F_GETLK/F_SETLK/F_SETLKW)

```c
#include <fcntl.h>
/* 文件状态值解析 */
void read_fd_state(int sta)
{
    switch (sta & O_ACCMODE)
    {
    case O_RDONLY:
    {
        printf("readonly");
        break;
    }
    case O_WRONLY:
    {
        printf("writeonly");
        break;
    }
    case O_RDWR:
    {
        printf("readwrite");
        break;
    }
    default:
    {
        printf("unknown");
        break;
    }
    }

    if (sta & O_APPEND)
        printf(",append");
    if (sta & O_NONBLOCK)
        printf(",nonblock");
    if (sta & O_SYNC)
        printf(",sync");
    if (sta & O_FSYNC)
        printf(",fsync");
    if (sta & O_CREAT)
        printf(",creat");

    printf("\n");
}

void test(void)
{
    char *dir = "file";

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        int fd_flag = fcntl(fd, F_GETFD);           // 1. 获取文件描述符标志
        printf("fd:%d fd_flag:%d\n", fd, fd_flag);
        fcntl(fd, F_SETFD, 1);                      // 2. 设置文件描述符标志，这里包括了FD_CLOEXEC标志

        int fd_new = fcntl(fd, F_DUPFD);            // 3. 复制文件描述符，清除了FD_CLOEXEC标志，该标志表示文件在执行时该描述符依然有效
        printf("fd:%d  fd_new:%d\n", fd, fd_new);

        int fd_new2= fcntl(fd, F_DUPFD_CLOEXEC);    // 4. 复制文件描述符，不清除FD_CLOEXEC标志
        printf("fd:%d  fd_new2:%d\n", fd, fd_new2);

        // 这里是测试F_DUPFD和F_DUPFD_CLOEXEC是否清除FD_CLOSE标志
        fd_flag = fcntl(fd, F_GETFD);
        printf("fd:%d fd_flag:%d\n", fd, fd_flag);
        fd_flag = fcntl(fd_new, F_GETFD);
        printf("fd_new:%d fd_flag:%d\n", fd_new, fd_flag);
        fd_flag = fcntl(fd_new2, F_GETFD);
        printf("fd_new2:%d fd_flag:%d\n", fd_new2, fd_flag);

        int fd_sta = fcntl(fd, F_GETFL);            // 5. 获取文件状态
        read_fd_state(fd_sta);

        fd_sta |= O_NONBLOCK;
        fd_sta |= O_SYNC;
        fcntl(fd, F_SETFL, fd_sta);                 // 6. 设置文件状态（测试不能更改O_SYNC，O_FSYNC标志）
        fd_sta = fcntl(fd, F_GETFL);                // 获取文件状态
        read_fd_state(fd_sta);

        int fd_own = fcntl(fd, F_GETOWN);           // 7. 获取当前接收SIGIO和SIGURG信号的进程ID或进程组ID，返回值小于0时，其绝对值表示进程组ID
        printf("fd_own:%d\n", fd_own);
        fcntl(fd, F_SETOWN, 2000);                  // 8. 设置当前接收SIGIO和SIGURG信号的进程ID或进程组ID

        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
        if (close(fd_new))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
        if (close(fd_new2))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}
```
**关于ioctl**

ioctl有非常多的命令来操作I/O,需要根据实际的情况来选择和使用命令

```c
#include <unistd.h>
#include <sys/ioctl.h>


// ...
```

#### 文件与目录

** 查看文件类型 **

使用stat、fstat、lstat来查看文件的类型

```c
#include <sys/stat.h>
void test(void)
{
    char *dir = "file";

    /**
    struct stat {
        mode_t     st_mode;       //文件对应的模式，文件，目录等
        ino_t      st_ino;        //inode节点号
        dev_t      st_dev;        //设备号码
        dev_t      st_rdev;       //特殊设备号码
        nlink_t    st_nlink;      //文件的连接数
        uid_t      st_uid;        //文件所有者
        gid_t      st_gid;        //文件所有者对应的组
        off_t      st_size;       //普通文件，对应的文件字节数
        time_t     st_atime;      //文件最后被访问的时间
        time_t     st_mtime;      //文件内容最后被修改的时间
        time_t     st_ctime;      //文件状态改变时间
        blksize_t st_blksize;     //文件内容对应的块大小
        blkcnt_t   st_blocks;     //文件内容对应的块数量
    };
    */

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        struct stat buf;
        char *ptr;

        struct stat buf3;
        if (stat(dir, &buf3) < 0)
            fprintf(stderr, "stat error:%s\n", strerror(errno));
        printf("文件长度:%d\n", buf3.st_size);
        printf("普通文件:%s\n", S_ISREG(buf3.st_mode) ? "T" : "F");
        printf("目录文件:%s\n", S_ISDIR(buf3.st_mode) ? "T" : "F");
        printf("字符特殊文件:%s\n", S_ISCHR(buf3.st_mode) ? "T" : "F");
        printf("块特殊文件:%s\n", S_ISBLK(buf3.st_mode) ? "T" : "F");
        printf("管道或FIFO:%s\n", S_ISFIFO(buf3.st_mode) ? "T" : "F");
        printf("符号链接:%s\n", S_ISLNK(buf3.st_mode) ? "T" : "F");
        printf("套接字:%s\n", S_ISSOCK(buf3.st_mode) ? "T" : "F");
        printf("消息队列:%s\n", S_TYPEISMQ(&buf3) ? "T" : "F");
        printf("信号量:%s\n", S_TYPEISSEM(&buf3) ? "T" : "F");
        printf("共享存储对象:%s\n", S_TYPEISSHM(&buf3) ? "T" : "F");
        printf("--------------------\n");

        if (lstat(dir, &buf) < 0)
            fprintf(stderr, "lstat error:%s\n", strerror(errno));
        printf("文件长度:%d\n", buf.st_size);
        printf("普通文件:%s\n", S_ISREG(buf.st_mode) ? "T" : "F");
        printf("目录文件:%s\n", S_ISDIR(buf.st_mode) ? "T" : "F");
        printf("字符特殊文件:%s\n", S_ISCHR(buf.st_mode) ? "T" : "F");
        printf("块特殊文件:%s\n", S_ISBLK(buf.st_mode) ? "T" : "F");
        printf("管道或FIFO:%s\n", S_ISFIFO(buf.st_mode) ? "T" : "F");
        printf("符号链接:%s\n", S_ISLNK(buf.st_mode) ? "T" : "F");
        printf("套接字:%s\n", S_ISSOCK(buf.st_mode) ? "T" : "F");
        printf("消息队列:%s\n", S_TYPEISMQ(&buf) ? "T" : "F");
        printf("信号量:%s\n", S_TYPEISSEM(&buf) ? "T" : "F");
        printf("共享存储对象:%s\n", S_TYPEISSHM(&buf) ? "T" : "F");
        printf("--------------------\n");

        struct stat buf2;
        if (fstat(fd, &buf2) < 0)
            fprintf(stderr, "fstat error:%s\n", strerror(errno));
        printf("文件长度:%d\n", buf2.st_size);
        printf("普通文件:%s\n", S_ISREG(buf2.st_mode) ? "T" : "F");
        printf("目录文件:%s\n", S_ISDIR(buf2.st_mode) ? "T" : "F");
        printf("字符特殊文件:%s\n", S_ISCHR(buf2.st_mode) ? "T" : "F");
        printf("块特殊文件:%s\n", S_ISBLK(buf2.st_mode) ? "T" : "F");
        printf("管道或FIFO:%s\n", S_ISFIFO(buf2.st_mode) ? "T" : "F");
        printf("符号链接:%s\n", S_ISLNK(buf2.st_mode) ? "T" : "F");
        printf("套接字:%s\n", S_ISSOCK(buf2.st_mode) ? "T" : "F");
        printf("消息队列:%s\n", S_TYPEISMQ(&buf2) ? "T" : "F");
        printf("信号量:%s\n", S_TYPEISSEM(&buf2) ? "T" : "F");
        printf("共享存储对象:%s\n", S_TYPEISSHM(&buf2) ? "T" : "F");
        printf("--------------------\n");

        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}
```

/** 文件权限 **/

文件有3类型拥有者:用户、组、其他
文件的每个拥有者有3类权限：读、写、执行
不同类型的权限可能会有不同的作用，规则如下：
1. 目录的读权限允许查看目录内所有的的文件名列表，目录的执行权限允许搜索文件名，目录的写权限和执行权限允许在目录中创建/删除文件
2. 读权限允许是否可以对文件执行读操作
3. 写权限允许是否可以对文件执行写操作
4. exec函数执行具有执行权限的普通文件

**使用access和facessat测试文件权限或是否存在**

```c
#include <unistd.h>
void test(void)
{
    char *dir = "file";

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        printf("文件是否存在:%s\n", !access(dir, F_OK) ? "T" : "F");
        printf("文件是否可读:%s\n", !access(dir, R_OK) ? "T" : "F");
        printf("文件是否可写:%s\n", !access(dir, W_OK) ? "T" : "F");
        printf("文件是否可执行:%s\n", !access(dir, X_OK) ? "T" : "F");

        printf("文件是否存在:%s\n", !faccessat(AT_FDCWD, dir, F_OK, AT_EACCESS) ? "T" : "F");
        printf("操作该文件的进程(有效用户ID或有效组ID)是否有可读权限:%s\n", !faccessat(AT_FDCWD, dir, R_OK, AT_EACCESS) ? "T" : "F");
        printf("操作该文件的进程(有效用户ID或有效组ID)是否有可写权限:%s\n", !faccessat(AT_FDCWD, dir, W_OK, AT_EACCESS) ? "T" : "F");
        printf("操作该文件的进程(有效用户ID或有效组ID)是否有可执行权限:%s\n", !faccessat(AT_FDCWD, dir, X_OK, AT_EACCESS) ? "T" : "F");
        
        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}
```

**使用umask屏蔽设置的权限**

使用umask可以让系统屏蔽一些权限，让这些权限无法被设置，umask没有错误返回值

```c
#include <sys/stat.h>
void test(void)
{
    umask(0);

    char *dir1 = "file1", *dir2 = "file2";

    if (creat(dir1, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));
    printf("文件是否存在:%s\n", !access(dir1, F_OK) ? "T" : "F");
    printf("文件是否可读:%s\n", !access(dir1, R_OK) ? "T" : "F");
    printf("文件是否可写:%s\n", !access(dir1, W_OK) ? "T" : "F");
    printf("文件是否可执行:%s\n", !access(dir1, X_OK) ? "T" : "F");

    /**
     * S_IRUSR S_IRGRP S_IROTH  拥有者/组/其他-读
     * S_IWUSR S_IWGRP S_IWOTH  拥有者/组/其他-写
     * S_IXUSR S_IXGRP S_IXOTH  拥有者/组/其他-执行
    */
    umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH | S_IXUSR);

    if (creat(dir2, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    /* 只能测试拥有者的权限 */
    printf("文件是否存在:%s\n", !access(dir2, F_OK) ? "T" : "F");
    printf("文件是否可读:%s\n", !access(dir2, R_OK) ? "T" : "F");
    printf("文件是否可写:%s\n", !access(dir2, W_OK) ? "T" : "F");
    printf("文件是否可执行:%s\n", !access(dir2, X_OK) ? "T" : "F");
}
```

**修改文件的权限**

使用chmod、fchmod、fchmodat来设置权限

```c
#include <sys/stat.h>
void test(void)
{
    char *dir1 = "file1";

    int fd = open(dir1, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        printf("文件是否存在:%s\n", !access(dir1, F_OK) ? "T" : "F");
        printf("文件是否可读:%s\n", !access(dir1, R_OK) ? "T" : "F");
        printf("文件是否可写:%s\n", !access(dir1, W_OK) ? "T" : "F");
        printf("文件是否可执行:%s\n", !access(dir1, X_OK) ? "T" : "F");

        if (chmod(dir1, 0555) < 0)
            fprintf(stderr, "Close error:%s\n", strerror(errno));

        if (fchmod(fd, 0666) < 0)
            fprintf(stderr, "fchmod error:%s\n", strerror(errno));

        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}
```

**修改用户ID和组ID**

使用chown、fchown、fchownat和lchown来修改用户ID和组ID

```c
#include <sys/stat.h>
void test(void)
{
    char *dir1 = "file1";

    int fd = open(dir1, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        // cat /etc/group查看组id
        // cat /etc/passwd查看用户id
        if (chown(dir1, 1000, 1000) < 0)
            fprintf(stderr, "Chown error:%s\n", strerror(errno));
        system("ls file1 -al");

        if (lchown(dir1, 1000, 1000) < 0)
            fprintf(stderr, "Chown error:%s\n", strerror(errno));
        system("ls file1 -al");

        if (fchown(fd, 1000, 1000) < 0)
            fprintf(stderr, "Chown error:%s\n", strerror(errno));    
        system("ls file1 -al");
        
        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}

```
**截断文件的内容**

使用truncate、ftruncate来截断文件的内容

```c
#include <sys/stat.h>
void test(void)
{
    char *dir = "file";

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        char *buf1 = "1234556";
        char buf2[sizeof(buf1)] = {0};

        if (write(fd, buf1, sizeof(buf1)) != sizeof(buf1))
            fprintf(stderr, "lseek error:%s\n", strerror(errno));
        
        if (pread(fd, buf2, sizeof(buf2), 0) < 0)
            fprintf(stderr, "lseek error:%s\n", strerror(errno));
        buf2[sizeof(buf2) - 1] = 0;
        printf("buf2:%s\n", buf2);

        // truncate截断文件(但是这里暂时没有生效，需要用ftruncate)
        truncate(dir, 5);

        if (pread(fd, buf2, sizeof(buf2), 0) < 0)
            fprintf(stderr, "lseek error:%s\n", strerror(errno));
        buf2[sizeof(buf2) - 1] = 0;
        printf("buf2:%s\n", buf2);

        // ftruncate截断文件
        ftruncate(fd, 6);

        if (pread(fd, buf2, sizeof(buf2), 0) < 0)
            fprintf(stderr, "lseek error:%s\n", strerror(errno));
        buf2[sizeof(buf2) - 1] = 0;
        printf("buf2:%s\n", buf2);

        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}
```

**硬链接和软链接**

硬链接一般不允许链接目录，且只能让同一文件系统间的文件链接。
软链接没有上述要求。

硬链接操作有:link、linkat、unlink
软链接操作有:symlink、symlinkat、unlink、readlink、readlinkat

```c
#include <unistd.h>
void test(void)
{
    char *dir1 = "file1", *dir1_link = "file1_link";
    char *dir2 = "file2", *dir2_symlink = "file2_symlink";

    if (creat(dir1, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));
    if (creat(dir2, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    // 硬链接
    if (link(dir1, dir1_link) < 0)
        fprintf(stderr, "Link error:%s\n", strerror(errno));

    // 软链接
    if (symlink(dir2, dir2_symlink) < 0)
        fprintf(stderr, "Link error:%s\n", strerror(errno));

    // 读软链接
    char buf[100];
    if (readlink(dir2_symlink, buf, sizeof(buf)) < 0)
        fprintf(stderr, "Readlink error:%s\n", strerror(errno));
    printf("Readlink res:%s\n", buf);

    struct stat st;
    lstat(dir1, &st);
    printf("Link number:%ld\n", st.st_nlink);
    lstat(dir2, &st);
    printf("Link number:%ld\n", st.st_nlink);

    // 删除硬链接
    // unlink(dir1_link);
    remove(dir1_link);
    // 删除软链接
    unlink(dir2_symlink);

    lstat(dir1, &st);
    printf("Link number:%ld\n", st.st_nlink);
    lstat(dir2, &st);
    printf("Link number:%ld\n", st.st_nlink);
}
```

**文件重命名**

使用rename、renameat来修改文件名

```c
#include <unistd.h>
void test(void)
{
    char *dir1 = "file1", *dir1_rename = "file1_rename";

    if (creat(dir1, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    // 重命名
    if (rename(dir1, dir1_rename) < 0)
        fprintf(stderr, "Rename error:%s\n", strerror(errno));

    if (!access(dir1, F_OK))
        printf("%s is exist!\n", dir1);
    else
        printf("%s is not exist!\n", dir1);

    if (!access(dir1_rename, F_OK))
        printf("%s is exist!\n", dir1_rename);
    else
        printf("%s is not exist!\n", dir1_rename);

    if (remove(dir1_rename) < 0)
        fprintf(stderr, "Remove error:%s\n", strerror(errno));
}
```

**文件时间的修改**

文件时间分为访问时间、修改时间，使用utimes(Linux不支持这个API)、ftimes、utimesat函数来更改这个时间。此外可以设置tv_nsec字段为UTIME_NOW或UTIME_OMIT来选择设置时间为当前时间、或不设置时间。

```c
#include <sys/stat.h>
void test(void)
{
    char *dir1 = "file1";

    int fd;
    fd = open(dir1, O_RDWR | O_CREAT| O_TRUNC, 0777);
    if (fd)
    {
        struct stat s;
        lstat(dir1, &s);

        struct timespec times[2];
        times[0] = s.st_atim;
        times[1] = s.st_mtim;

        /**
         * 使用futimens修改文件的访问时间和修改时间
         * 
         * struct timespec time;
         * time.tv_nsec设为UTIME_NOW时，futimens会忽略tv_sec字段，并将相应时间戳设为当前时间
         * time.tv_nsec设为UTIME_OMIT时，futimens会忽略tv_sec字段，并且不修改时间戳
         * 
         * ls -lu xxx   查看最后访问时间
         * ls -lc xxx   查看最后修改时间
        */
        futimens(fd, times);

        if (close(fd) < 0)
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }

    if (!access(dir1, F_OK))
    {
        if (remove(dir1) < 0)
            fprintf(stderr, "Remove error:%s\n", strerror(errno));
    }
}
```

**创建、删除目录**

使用mkdir、mkdirat创建目录，使用rmdir删除空目录
使用opendir、fdopendir打开目录
使用readdir读取目录
使用closedir关闭目录
使用rewinddir、seekdir、telldir操作目录指针位置

```c
#include <sys/types.h>
#include <dirent.h>

void test(void)
{
    char *dir1 = "file1_dir";
    char *file1 = "file1";

    if (mkdir(dir1, 0777) < 0)
        fprintf(stderr, "Mkdir error:%s\n", strerror(errno));
    
    /* 打开目录 */
    DIR *open_dir = NULL;
    struct dirent *dirp;
    open_dir = opendir(dir1);
    if (open_dir)
    {
        /* 查询目录 */
        rewinddir(open_dir);        // 目录指针恢复到起始位置
        seekdir(open_dir, 5830436974003520366);      // 修改目录指针
        while ((dirp = readdir(open_dir)) != NULL)
        if (dirp)
        {
            printf("目录指针:%ld\t", telldir(open_dir));
            printf("inode:%ld\t", dirp->d_ino);
            printf("名字:%s\t", dirp->d_name);
            printf("偏移:%d\t", dirp->d_reclen);
            printf("记录长度:%d\t", dirp->d_reclen);
            printf("类型:");
            switch (dirp->d_type)
            {
                case DT_BLK:printf("block dev\n");break;
                case DT_CHR:printf("character dev\n");break;
                case DT_DIR:printf("directory\n");break;
                case DT_FIFO:printf("pipe(fifo)\n");break;
                case DT_LNK:printf("symlink\n");break;
                case DT_REG:printf("regular file\n");break;
                case DT_SOCK:printf("socket\n");break;
                case DT_UNKNOWN:printf("unknown\n");break;
                default:printf("type is unknown\n");break;
            }
        }
        /* 关闭目录 */
        if (closedir(open_dir) < 0)
            fprintf(stderr, "Closedir error:%s\n", strerror(errno));
    }

    if (creat(file1, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    if (rmdir(dir1) < 0)
        fprintf(stderr, "Rmdir error:%s\n", strerror(errno));
        
    if (!access(file1, F_OK))
    {
        if (remove(file1) < 0)
            fprintf(stderr, "Remove error:%s\n", strerror(errno));
    }

    if (!access(dir1, F_OK))
    {
        if (remove(dir1) < 0)
            fprintf(stderr, "Remove error:%s\n", strerror(errno));
    }
}
```

**更改/设置当前工作目录**

使用fchdir、chdir设置当前工作目录，使用getcwd获取当前工作目录

```c
#include <unistd.h>

void test(void)
{
    if (chdir("/tmp") < 0)
        fprintf(stderr, "Chdir error:%s\n", strerror(errno));
    
    char buf[200];
    if (getcwd(buf, sizeof(buf)))
    {
        printf("getcwd:%s\n", buf);
    }
}
```

**获取主次设备号**

```c
#include <sys/stat.h>
#include <sys/sysmacros.h>
void test(void)
{
    char *dir = "file";
    if (creat(dir, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    struct stat buf;
    if (lstat(dir, &buf) < 0)
        fprintf(stderr, "Lstat error:%s\n", strerror(errno));

    /* 打印主次设备号 */
    printf("dev = %d/%d\n", major(buf.st_dev), minor(buf.st_dev));

    /* 打印字符/块设备实际的主/次设备号 */
    if (S_ISCHR(buf.st_mode) || S_ISBLK(buf.st_mode))
    {
        printf("%s rdev = %d/%d\n", 
        S_ISCHR(buf.st_mode) ? "character" : "block", 
        major(buf.st_rdev), 
        minor(buf.st_rdev));
    }
    
    remove(dir);
}
```

## 系统数据文件和信息

Linux运行过程会伴随大量与系统有关的数据文件

**口令文件**

由于历史的原因，Linux口令文件是/etc/passwd

```c
/* A record in the user database.  */
struct passwd
{
  char *pw_name;		/* Username.  */
  char *pw_passwd;		/* Hashed passphrase, if shadow database
                                   not in use (see shadow.h).  */
  __uid_t pw_uid;		/* User ID.  */
  __gid_t pw_gid;		/* Group ID.  */
  char *pw_gecos;		/* Real name.  */
  char *pw_dir;			/* Home directory.  */
  char *pw_shell;		/* Shell program.  */
};
```

1. 如果想要阻止用户登录，可以将Shell程序设置为/dev/null、/bin/false(返回失败)或/bin/true(返回成功)
2. nobody用户id为65534，组id为65534，不提供任何权限，目的是让任何人都可以登录到系统。

**阴影口令文件**

阴影口令文件是单向加密算法处理过的用户口令副本，文件路径为/etc/shadow。为了防止外部用明文口令生成加密口令，再不断与其用户的加密口令对比，最终比对成功后得到原始口令，阴影口令一般只能由root用户访问，也因此普通口令文件/etc/passwd可以由各个用户读取。

```c
/* A record in the shadow database.  */
struct spwd
  {
    char *sp_namp;		/* Login name.  */
    char *sp_pwdp;		/* Hashed passphrase.  */
    long int sp_lstchg;		/* Date of last change.  */
    long int sp_min;		/* Minimum number of days between changes.  */
    long int sp_max;		/* Maximum number of days between changes.  */
    long int sp_warn;		/* Number of days to warn user to change
				   the password.  */
    long int sp_inact;		/* Number of days the account may be
				   inactive.  */
    long int sp_expire;		/* Number of days since 1970-01-01 until
				   account expires.  */
    unsigned long int sp_flag;	/* Reserved.  */
  };
```

**组文件**

组文件/etc/group

```c
/* The group structure.	 */
struct group
  {
    char *gr_name;		/* Group name.	*/
    char *gr_passwd;	/* Password.	*/
    __gid_t gr_gid;		/* Group ID.	*/
    char **gr_mem;		/* Member list.	指向各用户指针的数组*/
  };
```
**附属组id**

每个用户除了拥有一个组以外，也可以拥有多个附属组。这是为了解决一个用户参与多个项目时，需要同时属于多个组。

**其他数据文件**

`/etc/nsswwitch.conf`，系统可以使用网络信息服务(NIS)来管理用户和组的数据库。系统通过配置文件/etc/nsswitch.conf来管理每一类信息

`/etc/protocols`,记录各网络服务器提供服务的数据文件

`/etc/protocols`,记录协议信息的数据文件

`/etc/networks`，记录网络信息的数据文件

**登录账户记录**

有两个文件来记录账户信息：
1. `/var/run/utmp`:记录当前登录到系统的用户
2. `/var/log/wtmp`:记录用户登录的注销的日志
```c
/* The structure describing an entry in the user accounting database.  */
struct utmp
{
  short int ut_type;		/* Type of login.  */
  pid_t ut_pid;			/* Process ID of login process.  */
  char ut_line[UT_LINESIZE];	/* Devicename.  */
  char ut_id[4];		/* Inittab ID.  */
  char ut_user[UT_NAMESIZE];	/* Username.  */
  char ut_host[UT_HOSTSIZE];	/* Hostname for remote login.  */
  struct exit_status ut_exit;	/* Exit status of a process marked
				   as DEAD_PROCESS.  */
/* The ut_session and ut_tv fields must be the same size when compiled
   32- and 64-bit.  This allows data files and shared memory to be
   shared between 32- and 64-bit applications.  */
#ifdef __WORDSIZE_TIME64_COMPAT32
  int32_t ut_session;		/* Session ID, used for windowing.  */
  struct
  {
    int32_t tv_sec;		/* Seconds.  */
    int32_t tv_usec;		/* Microseconds.  */
  } ut_tv;			/* Time entry was made.  */
#else
  long int ut_session;		/* Session ID, used for windowing.  */
  struct timeval ut_tv;		/* Time entry was made.  */
#endif

  int32_t ut_addr_v6[4];	/* Internet address of remote host.  */
  char __glibc_reserved[20];		/* Reserved for future use.  */
};
```

**系统标识**

一般情况使用uname来获取主机和操作系统的有关信息

struct utsname {
	char sysname[65];
	char nodename[65];
	char release[65];
	char version[65];
	char machine[65];
#ifdef _GNU_SOURCE
	char domainname[65];
#else
	char __domainname[65];
#endif
};

**时间和日期**

有一系列的函数来获取和设置时间：

```c
time_t time(time_t *calptr);

/**
 * CLOCK_REALTIME   实时系统时间
 * CLOCK_MONOTONIC  不带负跳数的实时系统时间
 * CLOCK_PROCESS_CPUTIME_ID 调用进程的CPU时间
 * CLOCK_THREAD_CPUTIME_ID  调用线程的CPU时间
*/
int clock_gettime(clockid_t clock_id, struct timespec *tsp);
int clock_getres(clockid_t clock_id, struct timespec *tsp);
int clock_settime(clockid_t clock_id, const struct timespec *tsp);

/**
 * 第二个参数只能是NULL
*/
int gettimeofday(struct timeval *restrict tp, void *restrict tzp);

/* 时间格式转换 */
struct tm *gmtime(const time_t *calptr);
struct tm *localtime(const time_t *calptr);
time_t mktime(struct tm *tmptr);

/* 时间格式打印 */
size_t strftime(char *dst, size_t dst_size, const char *fmt, const struct tm *tm);
size_t strftime_l (char *__restrict __s, size_t __maxsize,
			  const char *__restrict __format,
			  const struct tm *__restrict __tp,
			  __locale_t __loc);
char *strptime (const char *__restrict __s,const char *__restrict __fmt, struct tm *__tp);
```

## 进程控制


**命令行参数**

ISO C和POSIX.1要求argv[argc]是一个空指针，因此循环处理参数处理可以有两种写法：

```c
int main(int argc, char *argv[])
{
    /* 方法1 */
    for (int i = 0; i < argc ;i ++)
    {
        printf("par[%d]:%s\n", i, argv[i]);
    }

    /* 方法2 */
    for (int i = 0; argv[i] != NULL; i ++)
    {
        printf("par[%d]:%s\n", i, argv[i]);
    }
    return 0;
}
```

**C程序的存储控件布局**

```c
/**
 *  ___________________
 * |___________________| 命令行参数、环境变量
 * |                   | 栈，向下增长，栈底地址0xC0000000
 * |_ _ _ _ _ _ _ _ _ _|
 * |                   |
 * |                   |
 * |                   |
 * |_ _ _ _ _ _ _ _ _ _|
 * |___________________| 堆，向上增长
 * |                   |
 * |                   | 未初始化数据(bss)
 * |___________________|
 * |                   |
 * |                   | 初始化数据(data)
 * |___________________|
 * |                   |
 * |                   | 
 * |___________________| 代码(text),起始地址0x08048000
*/
```

程序加载到进程中一般有text段、data段和bss段，堆段和栈段由程序分配，其中text段和data段会被放在磁盘程序文件中，而bss段没有，因为内核需要在启动程序时将它们都设置为0。此外程序还有其他例如符号表的段、调试信息的段并不被加载到内存中。

使用size查看程序的text段、data段、bss段的大小。

```shell
## 第4列、第5列分别以十进制和十六进制标识3段总长度
lxo@ubuntu:~/lxolibs$ size /bin/ls
   text    data     bss     dec     hex filename
 128069    4688    4824  137581   2196d /bin/ls
```

**存储空间分配**

ISO C说明的3个用来动态分配存储空间的函数，malloc,calloc和realloc。用这3个函数可以动态分配堆内存的空间。

1. malloc，分配制定字节数的空间，初始值不确定
2. calloc，为指定数量指定长度的对象分配存储空间，初始值为0
3. realloc，增加或减少已分配的空间。增加空间时，指针可能会修改，初始值不确定。
4. free，释放分配的空间

**环境表/环境变量**

每个程序都会收到一张环境表，通过全局变量environ获取，这是一个字符串数组，数组末尾为NULL。

```c
int main(int argc, char *argv[])
{
    /* 读环境变量表 */
    extern char **environ;
    for (int i = 0; environ[i] != NULL; i ++)
    {
        printf("%d) %s\n", i, argv[i]);
    }

    /* 读/写环境变量 */
    putenv("USER_TEST=12312312313");
    printf("USER_TEST:%s\n", getenv("USER_TEST"));        
    putenv("USER_TEST");                          // 删除
    printf("USER_TEST:%s\n", getenv("USER_TEST"));  

    /* 读/写环境变量2 */   
    setenv("USER_TEST2", "123", 0);     /* 最后一个参数用来选择是否覆盖原参数 */
    printf("USER_TEST2:%s\n", getenv("USER_TEST2"));     
    unsetenv("USER_TEST2");
    printf("USER_TEST2:%s\n", getenv("USER_TEST2")); 
    return 0;
}
```

关于putenv和setenv的区别，putenv是传入环境变量的地址，一般传入全局变量地址，因此不需要分配内存。而setenv需要分配内存，传入新内存的地址。

**函数setjmp和函数longjmp实现跨函数跳转**

调用setjmp保存当前栈环境，调用longjmp跳转到保存的栈环境，此时setjmp返回非0。注意全局变量、静态变量和易失变量(volatile修饰)不会因为跳转而改变值。

```c
#include <string.h>
#include <setjmp.h>

jmp_buf jmpbuffer;

void long_jmp(void)
{
    longjmp(jmpbuffer, 1);
}

int main(int argc, char *argv[])
{
    char line[10];

    if (setjmp(jmpbuffer) != 0)
    {
        printf("jmp by longjmp()!\n");
    }
        
    while (fgets(line, sizeof(line), stdin) != NULL)
        long_jmp();

    return 0;
}
```

**函数getrlimit和函数setrlimit修改和查看进程资源**

进程有多个资源，每个资源都有两个限制，软限制和硬限制。通过getrlimit和setrlimit可以查看和修改这些限制。更改资源限制时需要遵循3条规则：
1. 任何进程都可以将软限制修改为小于或等于其硬限制的值
2. 任何进程都可以降低硬限制值，但必须大于或等于软限制的值。且这种改变对普通用户是不可逆的
3. 超级用户进程可以提高硬限制值

```c
#include <errno.h>
#include <sys/resource.h>
#define doit(str, name) pr_limits(str,#name, name);

static void pr_limits(char *str, char *name, int resource);

int main(int argc, char *argv[])
{
    doit("进程总的可用内存空间", RLIMIT_AS);
    doit("栈的最大字节长度", RLIMIT_STACK);
    doit("data段、bss段和堆总长度", RLIMIT_DATA);
    doit("CORE文件最大字节数", RLIMIT_CORE);
    doit("CPU时间的最大量值(秒)，超过限制会向进程发送SIGXCPU信号", RLIMIT_CPU);
    doit("可创建文件的最大字节长度。超过限制会像进程发送SIGXFSZ信号", RLIMIT_FSIZE);
    doit("进程使用mlock能过锁定在存储空间中的最大字节长度", RLIMIT_MEMLOCK);
    doit("进程为POSIX消息队列可分配的最大存储字节数", RLIMIT_MSGQUEUE);
    doit("nice值可设置的最大限制(影响优先限级)", RLIMIT_NICE);
    doit("进程可以打开的文件数", RLIMIT_NOFILE);
    doit("进程实际用户ID可拥有的最大子进程数", RLIMIT_NPROC);
    // doit("用户可同时打开的伪终端的数量", RLIMIT_NPTS);
    doit("最大驻内存集字节长度", RLIMIT_RSS);
    // doit("任意给定时间，一个用户可以占用套接字缓冲区的最大长度(字节)", RLIMIT_SBSIZE);
    doit("进程可排队的信号最大数量", RLIMIT_SIGPENDING);
    // doit("用户可消耗的交换空间的最大字节数", RLIMIT_SWAP);

    return 0;
}

static void pr_limits(char *str, char *name, int resource)
{
    struct rlimit limit = {0};
    unsigned long long lim;

    if (getrlimit(resource, &limit) < 0)
        fprintf(stderr, "Getrlimit error:%s\n", strerror(errno));
    printf("%s\t", name);

    printf("curr:");
    if (limit.rlim_cur == RLIM_INFINITY) {
        printf("(无限)\t");
    }
    else {
        lim = limit.rlim_cur;
        printf("%d\t", lim);
    }

    printf("max:");
    if (limit.rlim_max == RLIM_INFINITY) {
        printf("(无限)\t");
    }
    else {
        lim = limit.rlim_max;
        printf("%d\t", lim);
    } 
    printf("%s\n", str);
}
```

## 进程控制

**进程标识**

已被启动的进程拥有唯一的ID，这些ID可以在进程退出后被复用，一般使用延时复用算法。
ID=0的进程是调度进程，又被称为交换进程。该进程是内核的一部分，也被称为系统进程。
ID=1的进程是init进程，在系统自举结束时由内核调用，该进程的程序文件保存在/etc/init或/sbin/init。init进程会读取与系统有关的初始化文件(/etc/rc*文件，/etc/inittab文件，以及存在/etc/init.d目录的文件)。init进程是用户态下的以超级用户特权运行的第一个进程，该进程不会终止。
ID=2的进程是页守护进程，负责支持虚拟存储系统的分页操作，运行在内核态下。

获取进程的标识：
```c
#include <unistd.h>
int main(int argc, char *argv[])
{
    printf("进程ID:%d\n", getpid());
    printf("进程的父进程ID:%d\n", getppid());
    printf("进程的实际用户ID:%d\n", getuid());
    printf("进程的有效用户ID:%d\n", geteuid());
    printf("进程的实际组ID:%d\n", getgid());
    printf("进程的有效组ID:%d\n", geteuid());
    return 0;
}

```

**创建子进程**

使用fork()创建进程，fork()调用后会向子进程返回0，向父进程返回子进程的pid。此外fork执行后子进程是父进程的副本，它们共享代码段，但不共享进程数据空间、堆和栈(这部分页会被标记为只读)。通过硬件支持而出现的写时复用技术来隔离他们的数据:当子进程或父进程尝试写数据时，会触发硬件页错误，错误处理程序会复制该页并赋予读写权限，并且程序计数器会返回到上一个语句并再次尝试写数据。至此，子进程的数据和父进程的数据被分隔开。

```c
#include <errno.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    pid_t pid;
    pid = fork();
    if (pid < 0)
        fprintf(stderr, "fork error:%s\n", strerror(errno));
    else if (pid == 0)
    {
        /* Child process */
        printf("Child pid:%d\n", getpid());
    }
    else if (pid > 0)
    {
        printf("Father pid:%d  Child pid:%d\n", getpid(), pid);
    }

    /* 此时的代码段被共用 */
    printf("hello world!\n");

    return 0;
}
```

fork后文件缓冲区也会被复制
fork后打开的文件描述符会被复制，为了避免影响，父进程和子进程都需要关闭不使用的文件描述符，用来防止干扰对方。
fork还有其他属性会被继承：
- 实际用户ID、实际组ID、有效用户ID、有效组ID
- 附属组ID
- 进程组ID
- 会话ID
- 控制终端
- 设置用户ID标志和设置组ID标志
- 当前工作目录
- 根目录
- 文件模式创建屏蔽字
- 信号屏蔽和安排
- 对任意打开文件描述符的执行时关闭标志
- 环境
- 链接的共享存储段
- 存储映像
- 资源限制

父进程和子进程的区别：
- fork返回值不同
- 进程ID不同
- 子进程的tms_utime、tms_stime、tms_cutime和tms_ustime的值设为0
- 子进程不继承父进程设置的文件锁
- 子进程的未处理闹钟被清除
- 子进程的未处理信号集设置为空集

**退出子进程**

子进程退出后必须回收其资源，否则会成为僵死进程。

如果父进程被杀死，子进程的父进程pid会设置为1，1号进程会在子进程退出时回收资源
如果父进程存在，子进程退出后没有调用wait或waitpid回收子进程，那么子进程会变成僵死进程

进程终止的8种方式:

1. main函数返回
2. (由ISO C说明)调用exit
3. 调用_Exit(由ISO C说明)或_exit(由POSIX.1说明)
4. 最后一个线程从启动例程返回
5. 最后一个线程调用pthread_exit
6. (异常终止)调用abort
7. (异常终止)接收到信号
8. (异常终止)最后一个线程对取消请求做响应

进程正常或异常终止后，会向父进程发送SIGCHLD信号
exit()函数会调用fclose关闭所有打开的流，缓冲的所有数据都会被冲洗，写到文件上。
main函数调用ruturn(0)和exit(0)是等价的，如果main函数没有显示返回值，那么终止状态就是未确定的。

使用atexit设置终止处理程序:

ISO C规定一个进程登记最多32个函数，这些函数由exit自动调用，调用的顺序与登记的顺序相反，调用atexit函数登记。POSIX.1规定如果调用了exec函数族的函数，则会清除登记的退出处理函数。

```c
#include <stdlib.h>
void user_exit_handle111111(void)
{
    printf("111111111111\n", __LINE__);
}

void user_exit_handle222222(void)
{
    printf("222222222222\n", __LINE__);
}

void user_exit_handle333333(void)
{
    printf("333333333333\n", __LINE__);
}

int main(void)
{
    atexit(user_exit_handle111111);
    atexit(user_exit_handle222222);
    atexit(user_exit_handle333333);
    return 0;
}
```

wait和waitpid等待子进程退出

调用wait或waitpid会有3种情况：
1. 如果子进程都在运行中，则阻塞
2. 如果一个子进程已终止，并正等待父进程获取其终止状态，则取得该子进程终止状态后立即返回
3. 如果父进程没有任何子进程，则返回错误

对于子进程返回状态，可以用4个宏来获取子进程返回的原因：
```c
void pr_exit(int status)
{
    printf("Check child process exit status: ");
    if (WIFEXITED(status))
    {
        printf("正常终止，exit status:%d\n", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        printf("异常终止，exit status:%d%s\n",WTERMSIG(status),
                WCOREDUMP(status) ? "(core file generated)" : "");
    }
    else if (WIFSTOPPED(status))
    {
        printf("子进程暂停，signal number:%d\n", WSTOPSIG(status));
    }

#if 0
    else if (WIFCONTINUED(status))
    {
        printf("子进程从暂停恢复(仅用于waitpid)\n");
    }
#endif
}
```

wait的使用示例：

等待任意一个进程退出后，wait就会退出阻塞。

```c
int main(int argc, char *argv[])
{
    pid_t pid;
    int status;

    /* 进程正常退出测试 */
    if ((pid = fork()) < 0)
        fprintf(stderr, "fork error:%s\n", strerror(errno));
    else if (pid == 0)
        exit(7);

    if (wait(&status) != pid)
        fprintf(stderr, "wait error:%s\n", strerror(errno));
    pr_exit(status);

    /* 进程异常退出测试 */
    if ((pid = fork()) < 0)
        fprintf(stderr, "fork error:%s\n", strerror(errno));
    else if (pid == 0)
        abort();

    if (wait(&status) != pid)
        fprintf(stderr, "wait error:%s\n", strerror(errno));
    pr_exit(status);
    
    /* 进程异常退出测试 */
    if ((pid = fork()) < 0)
        fprintf(stderr, "fork error:%s\n", strerror(errno));
    else if (pid == 0)
        status /= 0;

    if (wait(&status) != pid)
        fprintf(stderr, "wait error:%s\n", strerror(errno));
    pr_exit(status);

    exit(0);
}
```

waitpid的使用示例：

waitpid有两个参数来控制其功能，pid参数来选择要等待的子进程pid的方式，option参数进一步控制一些特殊的等待方式。

```c
#include <sys/wait.h>

void pr_exit(int status)
{
    printf("Get process exit status: ");
    if (WIFEXITED(status))
    {
        printf("正常终止，exit status:%d\n", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        printf("异常终止，exit status:%d%s\n",WTERMSIG(status),
                WCOREDUMP(status) ? "(core file generated)" : "");
    }
    else if (WIFSTOPPED(status))
    {
        printf("子进程暂停，signal number:%d\n", WSTOPSIG(status));
    }

#if 0
    else if (WIFCONTINUED(status))
    {
        printf("子进程从暂停恢复(仅用于waitpid)\n");
    }
#endif
}

int main(int argc, char *argv[])
{
    pid_t pid;
    int status = 0;

    /* 进程正常退出测试 */
    if ((pid = fork()) < 0)
        fprintf(stderr, "fork error:%s\n", strerror(errno));
    else if (pid == 0)
    {
        if ((pid = fork()) < 0)
            fprintf(stderr, "fork error:%s\n", strerror(errno));
        else if (pid > 0)
            exit(0);

        sleep(2);
        printf("Sencond child , parent pid = %ld\n", getppid());
        exit(0);
    }

    /**
     * @brief 等待子进程退出
     * 
     * @param [in]  pid
     *              pid == -1, 等待任意子进程，此时wait与waitpid等效
     *              pid > 0, 等待进程id=pid的子进程
     *              pid == 0, 等待组id=pid的任意子进程
     *              pid < -1, 等待组id等于pid绝对值的任意子进程
     * @param [out] status  进程退出状态  
     * @param [in]  option  
     *              WCONTINUED  如果子进程停止后已经继续，但状态未报告，则返回其状态（停止后继续？？）
     *              WNOHANG     如果子进程不是立即可用，则waitpid不阻塞并且返回0
     *              WUNTRACED   如果子进程已经停止但没有报告，则返回其状态。
    */
    if (waitpid(pid, &status, 0) != pid)
        fprintf(stderr, "waitpid error:%s\n", strerror(errno));

    exit(0);
}
```

使用waitid等待进程退出

```c
    /**
     * @brief 等待子进程退出
     * 
     * @param [in]  idtype  
     *              P_PID   等待特定进程，id包含要等待子进程的进程id
     *              P_PGID  等待特定进程组中的任意一个子进程，id包含要等待子进程的进程组id
     *              P_ALL   等待任意子进程，id参数无效
     * @param [in]  id      进程id或组进程id
     * @param [in]  infop   进程状态的详细参数
     * @param [in]  option  
     *              WCONTINUED  如果一个进程曾今被停止，此后又被继续，但其状态为报告，则返回其状态
     *              WEXITED     等待已退出的进程
     *              WSTOPPED    如果子进程已经停止但是没有报告，则返回其状态
     *              (上面三个参数之一必须被指定)
     *              WNOHANG     如果子进程不是立即可用，则不阻塞，立即返回
     *              WNOWAIT     等待子进程，但不破坏子进程的退出状态，该子进程可以被wait，waitid，waitpid调用取得
     * @return
    */
    int waitid (idtype_t __idtype, __id_t __id, siginfo_t *__infop, int __options);
```

wait3和wait4函数

这两个函数同样是等待子进程退出，区别是多了一个参数来获取子进程使用的资源概况。

**父/子进程竞争**

父/子进程竞争关系也就是两个进程先后调用的关系，可以用一些宏来互相通知，同步执行。

示例：（宏的写法以后补上）
```c
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    int status = 0;

    if ((pid = fork()) < 0)
        fprintf(stderr, "fork error:%s\n", strerror(errno));
    else if (pid == 0)
    {
        WAIT_PARENT();
        printf("Child id:%d\n", getpid());
    }
    else if (pid > 0)
    {
        printf("Parent id:%d\n", getpid());
        TELL_CHILD(pid);
    }


    exit(0);
}
```

**exec函数族**

进程调用exec函数族后会加载程序的代码段、数据段、堆段、栈段，但不会新建一个进程。

```c
/**
 * @brief exec函数族
 * 下面的函数族只有execve是系统调用，其他都是库函数，最终会调用execve
 * @param [in]  path或fd或file  
 *              path 文件目录
 *              fd  文件描述符
 *              file  文件名
 *                  1. 如果文件名包含"/"则视为目录
 *                  2. 否则会在PATH环境变量中搜索可执行文件
 *                  3. 如果不是可执行文件，则认为是一个shell脚本，尝试调用/bin/sh并将文件名作为shell的输入
 * @param [in]  arg或argv    
 *               arg和...组合用来传入单独的参数,注意最后一个参数必须为NULL
 *               argv用来传入参数指针数组，以NULL结尾  
 * @param [in]  envp
 *               环境变量表，以NULL结尾 
*/

int execve (const char *__path, char *const __argv[],char *const __envp[]);
int execv (const char *__path, char *const __argv[]);
int execle (const char *__path, const char *__arg, ...);
int execl (const char *__path, const char *__arg, ...);
int execvp (const char *__file, char *const __argv[]);
int execlp (const char *__file, const char *__arg, ...);
int execvpe (const char *__file, char *const __argv[], char *const __envp[]);
int fexecve (int __fd, char *const __argv[], char *const __envp[]);
```

**更改用户id和更改组id**

关于3个用户id：实际用户id，有效用户id和保存设置用户id

实际用户id：当前登录系统的用户id
有效用户id：当前访问资源时用来决定权限的用户id，默认有效用户id与实际用户id相同，但在某些时候需要修改有效用户id来提高权限才能访问某些文件/或资源。
保存的设置用户id：在目标文件的设置用户id位启动时会用到，当访问目标文件时，可以将有效用户id设置位与目标文件所属的用户id相同，这样就可以拥有权限访问目标文件。目标文件是可执行文件时，如果需要访问当前进程的数据，会将目标执行文件所在进程的有效用户id有修改为本地进程的有效用户id，操作结束时目标进程需要恢复原本的有效用户id，此时就依赖保存设置用户id来恢复。保存设置用户id是有效用户id的副本。

通过setuid和setgid来设置实际用户id以及有效用户id和实际用户组id以及有效用户组id，目的是提高某些进程的权限，或者降低某些进程的权限。
通过seteuid和setegid来设置有效用户id和有效用户组id
通过setreuid交换用户id和有效用户id
通过setregid交换用户组id和用户组有效id

**解释器文件**

解释器文件的通用格式:
```shell
#! pathname [optional-argument]
```

使用execl执行解释器文件

```c
int main(int argc, char *argv[])
{
    pid_t pid;
    int status = 0;

    if ((pid = fork()) < 0)
        fprintf(stderr, "fork error:%s\n", strerror(errno));
    else if (pid == 0)
    {
        printf("Child id:%d\n", getpid());
        
        /* 执行解释器文件,第一个参数是解释器文件路径，第二个参数是解释器文件名，第三、四个参数是传给解释器文件的可选参数 */
        if (execl("./test_shell", "test_shell", "arg0", "arg1", NULL) < 0)
            fprintf(stderr, "execl error:%s\n", strerror(errno));
    }

    wait(NULL);

    exit(0);
}
```

注意解释器是解释器文件使用`#!`指定文件，解释器和解释器文件是不同的文件。

**system执行命令**

C代码可以使用system来快速执行某些命令，system实现中调用了fork、exec和waitpid，因此有3种返回值：
1. fork失败或waitpid返回除EINTR（阻塞引起的错误）之外的错误时，system返回-1，并设置errno来指示错误
2. 执行exec失败，返回值同exit(127)
3. 返回shell的终止状态

**获取用户名**

使用getlogin()获取当前登录的用户名

```c
int main(int argc, char *argv[])
{
    printf("user name:%s\n", getlogin());

    exit(0);
}
```

**进程调度**

进程通过nice值作为进程调用的优先级，nice值越小优先级越高。

设置/获取nice值的3个函数
```c
#include <unistd.h>
int nice(int incr);

#include <sys/sources.h>
int getpriority(int which, id_t who);
int setpriority(int which, id_t who, int value);
```

**进程时间**

使用times来获取时间，通过比较前后时间来获取进程的运行时间

```c
struct tms
  {
    clock_t tms_utime;		/* User CPU time.  */
    clock_t tms_stime;		/* System CPU time.  */

    clock_t tms_cutime;		/* User CPU time of dead children.  */
    clock_t tms_cstime;		/* System CPU time of dead children.  */
  };
```

获取进程时间的代码例子

```c
#include <stdlib.h>
#include <sys/times.h>

static void pr_times(clock_t, struct tms *, struct tms *);
static void do_cmd(char *);

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);
    printf("argc:%d\n", argc);
    for (int i = 0; i < argc - 1; i ++)
    {
        do_cmd(argv[i]);
    }
    
    exit(0);
}

static void do_cmd(char *cmd)
{
    struct tms tmsstart, tmsend;
    clock_t start, end;
    int status;

    printf("\ncommand: %s\n", cmd);

    if ((start = times(&tmsstart)) == -1)
        fprintf(stderr, "times error:%s\n", strerror(errno));
    
    if ((status = system(cmd)) == -1)
        fprintf(stderr, "system error:%s\n", strerror(errno));

    if ((end = times(&tmsend)) == -1)
        fprintf(stderr, "times error:%s\n", strerror(errno));

    pr_times(end - start, &tmsstart, &tmsend);
}

static void pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend)
{
    static long clktck = 0;

    if (clktck == 0)
        if ((clktck = sysconf(_SC_CLK_TCK)) < 0)
            fprintf(stderr, "sysconf error:%s\n", strerror(errno));
        
        printf(" Real time                          : %7.2f\n", real / (double) clktck);
        printf(" User CPU time                      : %7.2f\n", (tmsend->tms_utime - tmsstart->tms_utime) / (double)clktck);
        printf(" System CPU time                    : %7.2f\n", (tmsend->tms_stime - tmsstart->tms_stime) / (double)clktck);
        printf(" User CPU time of dead children     : %7.2f\n", (tmsend->tms_cutime - tmsstart->tms_cutime) / (double)clktck);
        printf(" System CPU time of dead children   : %7.2f\n", (tmsend->tms_cstime - tmsstart->tms_cstime) / (double)clktck);
}
```
## 其他

#### 修改运行环境路径

```c
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
void test(void)
{
    /* 查看工作路径 */
    char *curr_work_dir1 = getcwd(NULL, 0);
    printf("curr wrok dir:%s\n", curr_work_dir1);
    /* 修改工作路径 */
    if (chdir("/"))
        fprintf(stderr, "Chdir error:%s\n", strerror(errno));

    char *curr_work_dir2 = getcwd(NULL, 0);
    printf("curr wrok dir:%s\n", curr_work_dir2);
    if (chdir(curr_work_dir1))
        fprintf(stderr, "Chdir error:%s\n", strerror(errno));
    free(curr_work_dir1);
    free(curr_work_dir2);
}
```




#### 终端输入/输出

```c
#include <unistd.h>
#include <errno.h>
#include <string.h>
void test(void)
{
    int c;
    while ((c = getc(stdin)) != EOF)
    {
        if (putc(c, stdout) == EOF)
            fprintf(stderr, "Puts error:%s\n", strerror(errno));
    }

    if (ferror(stdin))
        fprintf(stderr, "Gets error:%s\n", strerror(errno));

}

void test(void)
{
    char buf[200];

    printf("%% ");
    while (fgets(buf, sizeof(buf), stdin) != NULL)
    {
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;

        fputs(buf, stdout);
    }
}
```

#### 进程控制

```c
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void test(void)
{
    char buf[200];
    pid_t pid;
    int status;

    printf("%% ");
    while (fgets(buf, sizeof(buf), stdin) != NULL)
    {
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;

        fputs(buf, stdout);

        if ((pid = fork()) < 0)
        {
            fprintf(stderr, "Fork error:%s\n", strerror(errno));
        }
        else if (pid == 0)
        {
            execlp(buf, buf, (char *) 0);
            fprintf(stderr, "Can't exec :%s\n", buf);
            exit(127);
        }

        if ((pid = waitpid(pid, &status, 0)) < 0)
            fprintf(stderr, "Waitpid error:%s\n", strerror(errno));
        printf("%% ");
    }
}

```
#### 用户标识

/etc/group  分组信息

```c
#include <unistd.h>
#include <sys/types.h>
void test(void)
{
    printf("uid:%d  gid:%d\n", getuid(), getgid()); 
}
```


#### 信号量使用

```c
#include <signal.h>
void sig_int_cb(int signo)
{
    printf("sig int callback\n");
    exit(0);
}

void test(void)
{
    if (signal(SIGINT, sig_int_cb) == SIG_ERR)
        fprintf(stderr, "Signal error:%s\n", strerror(errno));
}
```

#### 时间值

两种时间值：日历时间和进程时间

1. 日历时间
   UTC格式的世界时间,一般用time_t格式保存
2. 进程时间
   进程时间又称CPU时间，用来度量进程占用CPU资源的情况。以时钟滴答计算，一般用clock_t保存。
   进程时间又分为3类,可以使用`time`命令得到：
   - 时钟时间
   - 用户CPU时间：该进程执行用户指令使用的时间量
   - 系统CPU时间：该进程执行内核程序使用的时间量。例如read或write会调用内核程序。
  


#### 查看一些限制参数

Linux提供了一些宏来指示对参数的限制值，可以通过`sysconf`,`pathconf`,`fpathconf`查看。但注意某些限制值会在运行过程中改变，例如`_SC_OPEN_MAX`和`_PC_PATH_MAX`。如果要修改限制，可以用`ulimit`、`setrlimit`、`getrlimit`等函数修改，但是因为不常用就先不深究。

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void test(void)
{
    /* 查看系统的限制参数 */
    printf("Version:%ld\n", sysconf(_SC_VERSION));
    printf("XOPEN Version:%ld\n", sysconf(_SC_XOPEN_VERSION));
    printf("The size of page in bytes:%ld\n", sysconf(_SC_PAGE_SIZE));
    printf("Open file maximum num:%ld\n", sysconf(_SC_OPEN_MAX));
    printf("The number of clock ticks per second:%ld\n", sysconf(_SC_CLK_TCK));
    printf("ARG_MAX:%ld\n", sysconf(_SC_ARG_MAX));

    /* 查看某路径的限制参数 */
    char *dir = "/home/lxo";
    printf("_PC_NAME_MAX with %s :%ld\n", dir, pathconf(dir, _PC_NAME_MAX));
    printf("_PC_PATH_MAX with %s :%ld\n", dir, pathconf(dir, _PC_PATH_MAX));
    printf("_PC_LINK_MAX with %s :%ld\n", dir, pathconf(dir, _PC_LINK_MAX));
    printf("_PC_MAX_INPUT with %s :%ld\n", dir, pathconf(dir, _PC_MAX_INPUT));
    printf("_PC_MAX_CANON with %s :%ld\n", dir, pathconf(dir, _PC_MAX_CANON));
    printf("_PC_PIPE_BUF with %s :%ld\n", dir, pathconf(dir, _PC_PIPE_BUF));

    /* 查看某文件的限制参数 */
    int fd = open(dir, O_RDONLY);
    if (fd > 0)
    {
        printf("_PC_NAME_MAX :%ld\n", fpathconf(fd, _PC_NAME_MAX));
        printf("_PC_PATH_MAX :%ld\n", fpathconf(fd, _PC_PATH_MAX));
        printf("_PC_LINK_MAX :%ld\n", fpathconf(fd, _PC_LINK_MAX));
        printf("_PC_MAX_INPUT :%ld\n", fpathconf(fd, _PC_MAX_INPUT));
        printf("_PC_MAX_CANON :%ld\n", fpathconf(fd, _PC_MAX_CANON));
        printf("_PC_PIPE_BUF :%ld\n", fpathconf(fd, _PC_PIPE_BUF));

        close(fd);
    }
}
```