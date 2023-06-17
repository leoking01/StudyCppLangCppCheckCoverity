## [ 基于c++的日志文件实现 ](https://www.cnblogs.com/wangwenzhi2019/p/11100331.html)         

## 概述

所有的商业软件或线上系统都具有日志功能，因为日志信息提供了系统启动以来的重要的操作或状态迁移记录，是追踪各种异常错误的第一手资料。绝大部分系统的日志模块会自动保留历史日志文件，即：日志文件大小达到约定上限时，自动转储到一个新的历史文件，当前文件清空并继续记录新的日志信息，例如：假设当前日志文件名为test.log, 当它的大小到达上限（例如10MB)时，就把其文件内容转储到新文件test.log.1,  然后test.log清空并继续记录新信息。根据配置不同，我们可以保留1到N份历史日志文件。当历史日志文件数目达到上限时，我们可以采用[round-robin](https://en.wikipedia.org/wiki/Round-robin_scheduling)策略（或其他策略）依据文件生成时间依次覆盖老的文件。本文尝试用C++实现上述日志文件功能，其功能归纳总结如下：

1） 日志文件提供接口让用户配置日志文件名、日志大小上限、历史日志文件数目上限；

2） 日志文件提供Append()接口，让用户向文件追加日志消息；

3） 日志文件在执行Append()接口过程中，自动检测当前日志文件大小：如果追加当前消息后，文件大小超过约定上限，则记录当前消息前，将已有消息转储到历史文件并保证历史日志文件数不超过约定上限；否者，直接记录当前消息；

4） 日志文件提供接口让用户配置是否对历史日志文件进行压缩;

## 实现

LogFile类实现了上述功能，先看一下其接口定义：

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
 1 #ifndef _LOGFILE_H
 2 #define _LOGFILE_H
 3 
 4 #include <fstream>
 5 #include <iostream>
 6 
 7 
 8 class LogFile {
 9 public:
10     LogFile(const LogFile &) = delete;
11     LogFile& operator=(const LogFile &) = delete;
12     LogFile(const std::string&, double, unsigned int, bool);
13 
14     ~LogFile();
15     void Append(std::string &&msg);
16 
17 private:
18     void Rotate();
19     double GetFileSize();
20     std::string NextHistoryFile();
21 
22 private:
23     std::ofstream ofs_;
24     std::string file_name_;
25     double cur_size_;
26     double max_size_;
27     unsigned int max_file_num_;28     bool compress_;
28 };
29 
30 #endif //_LOGFILE_H
```

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

 

### **LogFile成员变量说明**

ofs_: c++ std::ofstream类型对象，通过其操作符"<<"把消息写入日志文件

file_name_: 日志文件名；

cur_size_: 实时记录当前日志文件大小，避免每次执行Append()操作时调用系统函数获取文件大小；

max_size_: 用户指定的日志文件上限；

max_file_num_: 用户指定的最大历史文件数；

 

### **LogFile成员函数说明**

**LogFile(const std::string&, double, unsigned int):** LogFile类的唯一构造函数，可以指定日志文件名，文件大小上限，历史文件数量上限，其实现也非常简单： 

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
LogFile::LogFile(const std::string& file_name,
                 double max_size,
                 unsigned int max_file_num,                 bool compress)
    : file_name_(file_name), max_size_(max_size), max_file_num_(max_file_num),      compress_(compress)
{
    assert(max_file_num_ > 0);
    cur_size_ = GetFileSize();
    ofs_.open(file_name_, std::ofstream::out|std::ofstream::app);
}
```

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

 

**~****LogFile()：** 析构函数，主要功能是关闭在构造函数中打开的ofstream对象.

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
LogFile::~LogFile()
{
    if (ofs_.is_open()) {
        ofs_.close();
    }
}
```

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

 

**void Append(std::string &&msg)：**LogFile类最重要的接口，让用户向日志文件追加新的消息。其实现逻辑为：在写文件前，先检查当前日志文件大小加上当前消息的长度之和是否会超过创建LogFile对象时指定的日志文件大小上限：如果超过，就将当前文件内容转存到历史文件，并清空当前日志文件（通过Rotate函数），然后，继续把当前消息写入日志文件。  有一点值得说明：获取当前日志文件大小不是通过调用系统函数，而是通过类成员变量cur_size_（构造函数调用一次系统函数为cur_size_赋初值，之后，每次执行Append(), cur_size_都累加消息长度，从而实时追踪日志文件长度）,  这样避免了每次执行Append()时都调用系统函数检查文件长度，从而提高了效率。

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
void LogFile::Append(std::string&& msg)
{
    double msg_size = (double)msg.size();
    if (cur_size_+ msg_size >= max_size_) {
        Rotate();
    }

    ofs_ << std::forward<std::string>(msg) << std::endl;
    cur_size_ += msg_size;
}
```

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)


**void Rotate()：** 转储函数，当前日志文件内容被转存到某个历史日志文件，当前日志文件被清空并被重新打开

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
//
// Compress file "old_file" and save data into file "new_file", the
// compression is based on zlib.
//
void LogFile::compress_file(const char *old_file, const char *new_file)
{
    gzFile gf = gzopen(new_file, "wb");
    if (gf == NULL) {
        std::cout << "gzopen() failed" << std::endl;
        return;
    }

    const int BUF_LEN = 500;
    char buf[BUF_LEN];
    size_t reads = 0;

    FILE *fd = fopen(old_file, "rb");
    if (fd == NULL) {
        std::cout << "fopen(" << old_file << ") failed" << std::endl;
    }

    while((reads = fread(buf, 1, BUF_LEN, fd)) > 0) {
        if (ferror(fd)) {
            std::cout << "fread() failed!" << std::endl;
            break;
        }
        gzwrite(gf, buf, reads);
    }

    fclose(fd);
    gzclose(gf);
}

//
// Save existing log messages into history file and empty log file.
//
// Note: call this function only when log file reaches maximum size.
//
void LogFile::Rotate()
{
    if (ofs_.is_open()) {
        ofs_.close();
    }

    std::string history_file = NextHistoryFile();
    if (compress_) {
        compress_file(file_name_.c_str(), history_file.c_str());
    } else {
        std::rename(file_name_.c_str(), history_file.c_str());
    }
    ofs_.open(file_name_, std::ofstream::out|std::ofstream::trunc);
    cur_size_ = 0;
}
     
```

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

 

**std::string NextHistoryFile():** 返回下一个历史日志文件名以用于转存日志文件信息

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
std::string LogFile::NextHistoryFile()
{
    static int next_file_no = 0;
    int file_num = (next_file_no++) % max_file_num_;
    return file_name_ + "." + std::to_string(file_num);
}
```

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

 

**double LogFile::GetFileSize()：** 返回当前日志文件大小(bytes)，当前实现只针对Linux

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
double LogFile::GetFileSize()
{
    struct stat statbuf;
    if (stat(file_name_.c_str(), &statbuf) == 0) {
        return (double)statbuf.st_size;
    } else {
        perror("Faild to get log file size");
        return 0;
    }
}
```

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

## 测试

针对上述LogFile的实现，我们可以进行如下测试：

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
#include "logfile.h"


int main(void)
{
    LogFile lf("testlogfile.log", 1024, 8, false);
    for (auto i = 0; i < 200; i++) {
        lf.Append(std::to_string(i) + ": this is a very very very very very very very very very very very very very very vvery very very very very ery long log");
    }
}
```

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

测试结果如下：

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
stephenw@stephenw-devbox1:/local/project/logcpp$ ls -lart testlogfile.log*
-rw-rw-r-- 1 stephenw stephenw 992 6月  27 23:02 testlogfile.log.4
-rw-rw-r-- 1 stephenw stephenw 992 6月  27 23:02 testlogfile.log.3
-rw-rw-r-- 1 stephenw stephenw 992 6月  27 23:02 testlogfile.log.2
-rw-rw-r-- 1 stephenw stephenw 992 6月  27 23:02 testlogfile.log.1
-rw-rw-r-- 1 stephenw stephenw 992 6月  27 23:02 testlogfile.log.7
-rw-rw-r-- 1 stephenw stephenw 992 6月  27 23:02 testlogfile.log.6
-rw-rw-r-- 1 stephenw stephenw 992 6月  27 23:02 testlogfile.log.5
-rw-rw-r-- 1 stephenw stephenw 992 6月  27 23:02 testlogfile.log.0
-rw-rw-r-- 1 stephenw stephenw 124 6月  27 23:02 testlogfile.log
```

stephenw@stephenw-devbox1:/local/project/logcpp$ tail -n5 testlogfile.log.0 testlogfile.log.1 testlogfile.log.2
==> testlogfile.log.0 <==
194: this is a very very very very very very very very very very very very very very vvery very very very very ery long log
195: this is a very very very very very very very very very very very very very very vvery very very very very ery long log
196: this is a very very very very very very very very very very very very very very vvery very very very very ery long log
197: this is a very very very very very very very very very very very very very very vvery very very very very ery long log
198: this is a very very very very very very very very very very very very very very vvery very very very very ery long log

==> testlogfile.log.1 <==
138: this is a very very very very very very very very very very very very very very vvery very very very very ery long log
139: this is a very very very very very very very very very very very very very very vvery very very very very ery long log
140: this is a very very very very very very very very very very very very very very vvery very very very very ery long log
141: this is a very very very very very very very very very very very very very very vvery very very very very ery long log
142: this is a very very very very very very very very very very very very very very vvery very very very very ery long log

==> testlogfile.log.2 <==
146: this is a very very very very very very very very very very very very very very vvery very very very very ery long log
147: this is a very very very very very very very very very very very very very very vvery very very very very ery long log
148: this is a very very very very very very very very very very very very very very vvery very very very very ery long log
149: this is a very very very very very very very very very very very very very very vvery very very very very ery long log
150: this is a very very very very very very very very very very very very very very vvery very very very very ery long log

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

 

 **总结**

 本文通过一个简单得c++类实现了日志文件主要功能，但仍然有许多提升空间，例如各种异常的处理（文件无法打开，写文件失败等），多线程以及跨平台的支持等，但仍不失为一个好的起点，稍加改造即可应用于生产环境。
