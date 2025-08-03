#pragma once

/*
    提供枚举类,枚举出日志等级
    提供转换接口:将枚举转换为对应字符串

*/

namespace msylog
{
    class LogLevel
    {
        public:
            enum class value
            {
                UNKNOW = 0,
                DEBUG,
                INFO,
                WARN,
                ERROR,
                FATAL
            };
            static const char* toString(LogLevel::value level)
            {
                switch(level)
                {
                    case LogLevel::value::DEBUG:return "DEBUG";
                    case LogLevel::value::INFO:return "INFO";
                    case LogLevel::value::WARN:return "WARN";
                    case LogLevel::value::ERROR:return "ERROR";
                    case LogLevel::value::FATAL:return "FATAL";
                    default:return "UNKNOW";
                }
            }
    };
}