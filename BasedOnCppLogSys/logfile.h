

#ifndef _LOGFILE_H
#define _LOGFILE_H

#include <fstream>
#include <iostream>


class LogFile {
public:
    LogFile(const LogFile &) = delete;
    LogFile& operator=(const LogFile &) = delete;
    LogFile(const std::string&, double, unsigned int, bool);

    ~LogFile();
    void Append(std::string &&msg);

private:
    void Rotate();
    double GetFileSize();
    std::string NextHistoryFile();

private:
    std::ofstream ofs_;
    std::string file_name_;
    double cur_size_;
    double max_size_;
    unsigned int max_file_num_;
    bool compress_;

    void LogFile::compress_file(const char *old_file, const char *new_file);
};

#endif //_LOGFILE_H
