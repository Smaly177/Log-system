#pragma once

/*
    日志消息类
    存储日志消息的所有要素
    1.日志的输出时间
    2.日志等级
    3.源文件名称
    4.源代码行号
    5.线程Id
    6.主体消息
    7.日志器名称
    [2003-08-16 12:38:26][root][12345678][main.cc:99][FATAL]:创建套接字失败

*/

#include "LogLevel.hpp"
#include "LogUtil.hpp"

#include <iostream>
#include <string>
#include <thread>

namespace msylog
{
    struct LogMessage
    {
        time_t _ctime;//日志产生的时间戳
        LogLevel::value _level;//日志等级
        size_t _line;//行号
        std::string _file;//文件名
        std::thread::id _tid;//线程id
        std::string _logger;//日志器名称
        std::string _payload;//主体消息
        
        LogMessage(LogLevel::value level,
                size_t line,
                const std::string file,
                const std::string logger,
                const std::string msg)
                :_ctime(util::Date::getTime())//时间
                ,_level(level)//等级
                ,_file(file)//文件名
                ,_line(line)//行号
                ,_tid(std::this_thread::get_id())//线程Id
                ,_logger(logger)//日志器名称  
                ,_payload(msg)//主体消息
                {}
    };
}
