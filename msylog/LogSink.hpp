#pragma once

/*
    日志落地模块实现
    1.抽象落地基类
    2.派生子类
    3.使用工厂模式表示创建与表示的分离

*/

#include "LogUtil.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <cassert>
#include <sstream>
#include <iomanip> 

namespace msylog
{  
    //落地方式抽象类
    class LogSink
    {
        public:
           using ptr = std::shared_ptr<LogSink>;
        //    virtual ~LogSink(); //为了方便支持扩展,将析构设置虚函数
           virtual void log(const char* data,size_t len) = 0;
    };
    //落地方式1:标准输出
    class StdoutSink : public LogSink
    {
        public:
            void log(const char* data,size_t len)
            {
                std::cout.write(data,len);
            }
    };
    //落地方式2:文件输出
    class FileSink : public LogSink
    {
        public:
            //构造时传入文件名,并打开文件,避免频繁打开,将操作句柄管理起来
            FileSink(const std::string& pathname)
            :_pathname(pathname)
            {
                //创建path路径目录,创建并打开日志文件
                util::File::createDirectory(util::File::getFilePath(_pathname));
                _ofs.open(_pathname,std::ios::binary | std::ios::app);
                assert(_ofs.is_open());
            }
            void log(const char* data,size_t len)
            {
                _ofs.write(data,len);
                assert(_ofs.good());
            }
        private:
            std::string _pathname;
            std::ofstream _ofs;
    };
    class RollBySizeSink : public LogSink
    {
        public:
            RollBySizeSink(const std::string& basename,size_t max_size)
            {
                _basename = basename;
                _max_size = max_size;
                _cur_fsize = 0;

                std::string pathname = createNewFile();

                util::File::createDirectory(util::File::getFilePath(pathname));

                _ofs.open(pathname,std::ios::binary | std::ios::app);

                assert(_ofs.is_open());
            }
            void log(const char* data,size_t len)
            {
                // 确保文件大小计算准确
                _ofs.seekp(0, std::ios::end);
                _cur_fsize = _ofs.tellp();
                
                if(_cur_fsize >= _max_size)
                {
                    std::string pathname = createNewFile();
                    _ofs.close();
                    _ofs.open(pathname,std::ios::binary | std::ios::app);
                    _cur_fsize = 0;
                    assert(_ofs.is_open());
                }
                _ofs.write(data,len);
                _ofs.flush(); // 确保数据写入文件
                _cur_fsize += len;
                assert(_ofs.good());
            }
            std::string createNewFile()//进行大小判断,超过指定大小创建新文件
            {
                static int count = 1;
                time_t t = util::Date::getTime();
                struct tm lt;
                localtime_r(&t,&lt);
                std::stringstream filename;
                filename<<_basename;
                // 年份：加上1900
                filename << (lt.tm_year + 1900);
                // 月份：加上1，并且确保有两位数字
                filename << std::setw(2) << std::setfill('0') << (lt.tm_mon + 1);
                // 日期、小时、分钟、秒：确保都是两位数字
                filename << std::setw(2) << std::setfill('0') << lt.tm_mday;
                filename << std::setw(2) << std::setfill('0') << lt.tm_hour;
                filename << std::setw(2) << std::setfill('0') << lt.tm_min; // 补上分钟
                filename << std::setw(2) << std::setfill('0') << lt.tm_sec;
                filename<<"__"<<count++;
                filename<<".log";
                return filename.str();
            }
        private:
            std::string _basename;//basename+编号代表文件名
            std::ofstream _ofs;
            size_t _max_size;
            size_t _cur_fsize;//记录当前文件写入数据大小
    };

    class SinkFactory
    {
        public:
            template <class SinkType, class... Args>
            static LogSink::ptr create(Args &&...args)
            {
                return std::make_shared<SinkType>(std::forward<Args>(args)...);
            }
    };  

}
