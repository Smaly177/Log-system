#pragma once
#include <iostream>
#include <ctime>
#include <unistd.h>
#include <sys/stat.h>
namespace msylog
{
    namespace util
    {




    /*
        日志项目中用到的一些实用工具类

            1.获取系统时间
            2.判断文件是否存在
            3.获取文件所在路径
            4.创建目录
    */

////////////////////////////////////////////////////////////////////
        class Date
        {
            public:
                static size_t getTime()
                {
                    return (size_t)time(nullptr);//返回当前时间
                }
        };
////////////////////////////////////////////////////////////////////
        class File
        {
            public:
                static bool exists(const std::string& pathname)
                {
                    // return (access(pathname.c_str(),F_OK)==0);
                    //由于不可跨平台,是posix标准库函数,我们不用这个
                    struct stat st;
                    if(stat(pathname.c_str(),&st)<0)
                        return false;
                    return true;
                }
                static std::string getFilePath(const std::string& pathname)
                {
                    //获取文件路径,不带文件名的
                    size_t pos = pathname.find_last_of("/\\");
                    if(pos == std::string::npos)
                        return ".";
                    return pathname.substr(0,pos+1);//带上/一块截取出来 
                }
                static void createDirectory(const std::string& pathname)
                {
                    size_t pos = 0;
                    size_t idx = 0;
                    while(idx<pathname.size())
                    {
                        pos = pathname.find_first_of("/\\",idx);
                        if(pos == std::string::npos)
                            mkdir(pathname.c_str(),0777);
                        std::string parent_dir = pathname.substr(0,pos+1);
                        // if(parent_dir == "." || parent_dir == "..") {idx = pos+1;continue;}  
                        if(exists(parent_dir) == true)
                            {idx = pos+1;continue;}    
                        mkdir(parent_dir.c_str(),0777);
                        idx = pos+1;                    
                    }
                }
        };














    }
}