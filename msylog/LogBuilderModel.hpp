#pragma once

/*
    LogLogger模块 已经对一些模块进行了整合,但是还需要我们自己去设置一些属性传参构造函数进去
    我们用建造者模式来简化操作,使用建造者模式来构造LogLogger模块所需参数
    用Build成员函数返回一个LogLogger的Logger类的指针对象

    //在这里构造完,然后返回,不需要自己设置了,中介功能

    这里我们对建造顺序没有要求,所以不需要指挥者了

        抽象一个建造者类
            设置日志器类型,将不同类型日志器的创建放到同一个日志器建造者类完成
        派生具体的建造者类 
            局部日志器的建造者&&全局日志器的建造者(后边添加了全局单例管理对象之后将日志器添加全局管理)
*/

#include "LogLogger.hpp"

#include <stdarg.h>
#include <cassert>
#include <unordered_map>
#include <mutex>


namespace msylog
{
    enum class LoggerType
{
    LOGGER_SYNC,
    LOGGER_ASYNC
};
    class LoggerBuilder
    {
        public:
            LoggerBuilder()
            :_logger_type(LoggerType::LOGGER_SYNC)//默认属性
            ,_limit_level(LogLevel::value::DEBUG)//默认属性
            ,_asynctype(AsyncType::ASYNC_SAFE)//默认属性
            {
            }
            void buildAsyncTypeSafe() //异步专属
            {
                _asynctype = AsyncType::ASYNC_SAFE;
            }
            void buildAsyncTypeUnSafe()
            {
                _asynctype = AsyncType::ASYNC_UNSAFE;
            }
            void buildLoggerType(LoggerType type)
            {
                _logger_type = type;
            }
            void buildLoggerName(const std::string& name)
            {
                _logger_name = name;
            }
            void buildLoggerLevel(LogLevel::value level)
            {
                _limit_level = level;
            }
            void buildLoggerFormatter(const std::string & pattern = "[%d{%Y-%m-%d %H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n")
            {
                _formatter = std::make_shared<Formatter>(pattern);
            }

            template<class SinkType,class ...Args>
            void buildSink(Args &&...args)
            {
                LogSink::ptr psink = SinkFactory::create<SinkType>(std::forward<Args>(args)...);
                _sinks.push_back(psink);
            }

            virtual Logger::ptr build() = 0;

        protected:
            AsyncType _asynctype;//异步专属

            std::string _logger_name;
            LoggerType _logger_type;
            std::atomic<LogLevel::value> _limit_level;
            Formatter::ptr _formatter;
            std::vector<LogSink::ptr> _sinks;
    };



    //局部 限制于作用域了
    class LocalLoggerBuilder: public LoggerBuilder
    {
        Logger::ptr build() override
        {
            assert(!_logger_name.empty()); //必须有日志器名称
            if(_formatter.get()==nullptr)
            {
                _formatter = std::make_shared<Formatter>();
            }
            if(_sinks.empty())
            {
                buildSink<StdoutSink>();
            }
            if(_logger_type == LoggerType::LOGGER_ASYNC)
            {
                //TODO..  have finished
                return std::make_shared<AsyncLogger>(_logger_name,_limit_level,_formatter,_sinks,_asynctype);
            }
            else
            {
                return std::make_shared<SyncLogger>(_logger_name,_limit_level,_formatter,_sinks);
            }

        }
    };

    
    class LoggerManager
    {
        public: 
            void addLogger(Logger::ptr logger)
            {
                if(hasLogger(logger->getname()))return ;
                std::unique_lock<std::mutex>lock (_mutex);
                _loggers.insert(std::make_pair(logger->getname(),logger));
            }
            bool hasLogger(const std::string& name)
            {
                std::unique_lock<std::mutex>lock (_mutex);
                auto it = _loggers.find(name);
                if(it!=_loggers.end())
                {
                    return true;
                }
                return false;
            }
            Logger::ptr getRootLogger()
            {
                return _root_logger;
            }
            Logger::ptr getLogger(const std::string& name)
            {
                std::unique_lock<std::mutex>lock (_mutex);
                auto it = _loggers.find(name);
                if(it==_loggers.end())
                {
                    return Logger::ptr();
                }
                return it->second;
            }
            static LoggerManager& getInstance()
            {
                static LoggerManager loggermanager; 
                return loggermanager; 
            }
        private:
            LoggerManager()
            {
                std::unique_ptr<LoggerBuilder> builder(new LocalLoggerBuilder());
                builder->buildLoggerName("默认日志器");
                builder->buildLoggerLevel(LogLevel::value::DEBUG);
                builder->buildLoggerType(LoggerType::LOGGER_SYNC);
                builder->buildLoggerFormatter();
                _root_logger = builder->build();
                _loggers.insert(std::make_pair("默认日志器",_root_logger));
            }
            LoggerManager(const LoggerManager&) = delete;
            LoggerManager operator=(const LoggerManager&) = delete;
            std::mutex _mutex;
            Logger::ptr _root_logger;//默认日志器
            std::unordered_map<std::string,Logger::ptr> _loggers;
    };
    
    //全局 单例模式 没有作用域限制，任何地方都可以日志输出
    //设置一个全局日志器的建造者--在局部的基础上新增了一个功能：将日志器添加到单例对象中
    class GlobalLoggerBuilder: public LoggerBuilder
    {
        Logger::ptr build() override
        {
            assert(!_logger_name.empty()); //必须有日志器名称
            if(_formatter.get()==nullptr)
            {
                _formatter = std::make_shared<Formatter>();
            }
            if(_sinks.empty())
            {
                buildSink<StdoutSink>();
            }
            Logger::ptr logger;
            if(_logger_type == LoggerType::LOGGER_ASYNC)
            {
                //TODO..  have finished
                logger = std::make_shared<AsyncLogger>(_logger_name,_limit_level,_formatter,_sinks,_asynctype);
            }
            else
            {
                logger = std::make_shared<SyncLogger>(_logger_name,_limit_level,_formatter,_sinks);
            }
            LoggerManager::getInstance().addLogger(logger);
            return logger;
        }
    };


}
