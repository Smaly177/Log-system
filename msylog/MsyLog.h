#include "LogBuilderModel.hpp"


    msylog::Logger::ptr getLogger(const std::string &name)
    {
        return msylog::LoggerManager::getInstance().getLogger(name);
    }
    msylog::Logger::ptr getrootLogger()
    {
        return msylog::LoggerManager::getInstance().getRootLogger();
    }

    // 使用宏函数进行打印（指定日志器）需要指定日志器指针->
    #define debug(fmt, ...) debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define info(fmt, ...) info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define error(fmt, ...) error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define warn(fmt, ...) warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define fatal(fmt, ...) fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

    // 使用宏函数进行打印(默认日志器) 不需要其他东西 //注意这里的debug也进行了宏替换 所以不用传入file line
    #define DEBUG(fmt, ...) getrootLogger()->debug(fmt, ##__VA_ARGS__)
    #define INFO(fmt, ...) getrootLogger()->info(fmt, ##__VA_ARGS__)
    #define ERROR(fmt, ...) getrootLogger()->error(fmt, ##__VA_ARGS__)
    #define WARN(fmt, ...) getrootLogger()->warn(fmt, ##__VA_ARGS__)
    #define FATAL(fmt, ...) getrootLogger()->fatal(fmt, ##__VA_ARGS__)