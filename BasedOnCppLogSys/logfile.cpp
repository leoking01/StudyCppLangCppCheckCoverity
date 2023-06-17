#include "logfile.h"
#include "assert.h"
LogFile::LogFile(const std::string& file_name,
                 double max_size,
                 unsigned int max_file_num,
                 bool compress)
    : file_name_(file_name), max_size_(max_size), max_file_num_(max_file_num),
      compress_(compress)
{
    assert(max_file_num_ > 0);
    cur_size_ = GetFileSize();
    ofs_.open(file_name_, std::ofstream::out|std::ofstream::app);
}


LogFile::~LogFile()
{
    if (ofs_.is_open()) {
        ofs_.close();
    }
}


#include <iostream>
#include <string>
using namespace std;

void LogFile::Append(std::string&& msg)
{
    double msg_size = (double)msg.size();
    if (cur_size_+ msg_size >= max_size_) {
        Rotate();
    }

    ofs_ << std::forward<std::string>(msg) << std::endl;
    cur_size_ += msg_size;
}

#include <zlib.h>
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

std::string LogFile::NextHistoryFile()
{
    static int next_file_no = 0;
    int file_num = (next_file_no++) % max_file_num_;
    return file_name_ + "." + std::to_string(file_num);
}


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
