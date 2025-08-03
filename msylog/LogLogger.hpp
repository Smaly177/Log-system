#pragma once

/*
    日志器模块
        对前边所有的模块进行整合,向外提供接口,不需要用户自己写一堆东西使用
            管理成员:
                格式化模块对象
                落地模块对象数组(一个日志器可能同时需要多个落地方式)
                输出限制等级(大于等于限制等级才会输出)
                互斥锁(保证线程安全)
                日志器名称(方便查找)
            提供的操作:
                debug等级日志的输出操作
                info等级日志的输出操作
                ....
    
            实现:
                抽象Logger基类(派生出 同步/异步 日志器类)
                因为两种日志器,只有落地方式不同,所以将落地方式抽象出来
 
*/

#include "LogUtil.hpp"
#include "LogLevel.hpp"
#include "LogFormat.hpp"
#include "LogSink.hpp"
#include "LogMessage.hpp"
#include "AsyncBuffer.hpp"
#include "AsyncLooper.hpp"

#include <atomic>
#include <mutex>
#include <stdarg.h>

namespace msylog
{
    class Logger
    {
        public:
            using ptr = std::shared_ptr<Logger>;
            Logger(const std::string& logger_name,
                LogLevel::value limit_level,
                Formatter::ptr& formatter,
                std::vector<LogSink::ptr>& sinks)
                :_logger_name(logger_name)
                ,_limit_level(limit_level)
                ,_formatter(formatter)
                ,_sinks(sinks.begin(),sinks.end())
                {}
            /*完成构造日志消息对象过程并进行格式化,得到格式化后的日志消息字符串--然后进行输出
              输出由另外接口进行*/
            void debug(const std::string& file,size_t line,const std::string& fmt,...)
            {
                //1.判断等级是否符合
                if(LogLevel::value::DEBUG<_limit_level) return;
                //2.参数包展开拿到消息
                va_list ap;
                va_start(ap,fmt);
                char* res;
                int ret = vasprintf(&res,fmt.c_str(),ap);
                if(ret == -1)
                {
                    std::cout<<"vasprintf failed!\n";
                    return ;
                }
                va_end(ap);
                //3.根据现有信息封装LogMessage对象
                LogMessage message(LogLevel::value::DEBUG,line,file,_logger_name,res);
                //4.格式化器
                std::stringstream ss;
                _formatter->format(ss,message);
                            // std::string msg = _formatter->format(message);
                //5.进行日志落地,交给另外接口log去做
                log(ss.str().c_str(),ss.str().size());

                free(res);
            }
            void info(const std::string& file,size_t line,const std::string& fmt,...)
            {
                if(LogLevel::value::INFO<_limit_level) return;
                va_list ap;
                va_start(ap,fmt);
                char* res;
                int ret = vasprintf(&res,fmt.c_str(),ap);
                if(ret == -1)
                {
                    std::cout<<"vasprintf failed!\n";
                    return ;
                }
                va_end(ap);
                LogMessage message(LogLevel::value::INFO,line,file,_logger_name,res);
                std::stringstream ss;
                _formatter->format(ss,message);
                log(ss.str().c_str(),ss.str().size());
                free(res);
            }
            void warn(const std::string& file,size_t line,const std::string& fmt,...)
            {
                if(LogLevel::value::WARN<_limit_level) return;
                va_list ap;
                va_start(ap,fmt);
                char* res;
                int ret = vasprintf(&res,fmt.c_str(),ap);
                if(ret == -1)
                {
                    std::cout<<"vasprintf failed!\n";
                    return ;
                }
                va_end(ap);
                LogMessage message(LogLevel::value::WARN,line,file,_logger_name,res);
                std::stringstream ss;
                _formatter->format(ss,message);
                log(ss.str().c_str(),ss.str().size());
                free(res);
            }
            void error(const std::string& file,size_t line,const std::string& fmt,...)
            {
                if(LogLevel::value::ERROR<_limit_level) return;
                va_list ap;
                va_start(ap,fmt);
                char* res;
                int ret = vasprintf(&res,fmt.c_str(),ap);
                if(ret == -1)
                {
                    std::cout<<"vasprintf failed!\n";
                    return ;
                }
                va_end(ap);
                LogMessage message(LogLevel::value::ERROR,line,file,_logger_name,res);
                std::stringstream ss;
                _formatter->format(ss,message);
                log(ss.str().c_str(),ss.str().size());
                free(res);
            }
            void fatal(const std::string& file,size_t line,const std::string& fmt,...)
            {
                if(LogLevel::value::FATAL<_limit_level) return;
                va_list ap;
                va_start(ap,fmt);
                char* res;
                int ret = vasprintf(&res,fmt.c_str(),ap);
                if(ret == -1)
                {
                    std::cout<<"vasprintf failed!\n";
                    return ;
                }
                va_end(ap);
                LogMessage message(LogLevel::value::FATAL,line,file,_logger_name,res);
                std::stringstream ss;
                _formatter->format(ss,message);
                log(ss.str().c_str(),ss.str().size());
                free(res);
            }
            std::string getname()
            {
                return _logger_name;
            }
            protected:
            virtual void log(const char* data,size_t len) = 0;
        protected:
            std::string _logger_name; //日志器名称
            std::atomic<LogLevel::value> _limit_level; //限制等级
            Formatter::ptr _formatter; //格式化器
            std::vector<LogSink::ptr> _sinks; //落地方式
            std::mutex _mutex;
    };
    class SyncLogger : public Logger
    {
        public:
            SyncLogger(const std::string& logger_name,
                LogLevel::value limit_level,
                Formatter::ptr& formatter,
                std::vector<LogSink::ptr>& sinks)
                :Logger(logger_name,limit_level,formatter,sinks)
                {}
        private:
            void log(const char* data,size_t len)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                if(_sinks.empty()) return;
                for(auto& sink:_sinks)
                {
                    sink->log(data,len);
                }
            }
    };

    class AsyncLogger:public Logger
    {
        public:
            using Functor = std::function<void(Buffer&)>;
            AsyncLogger(const std::string& logger_name,
            LogLevel::value limit_level,
            Formatter::ptr& formatter,
            std::vector<LogSink::ptr>& sinks,
            AsyncType asynctype = AsyncType::ASYNC_SAFE)
            :Logger(logger_name,limit_level,formatter,sinks)
            ,_looper(std::make_shared<AsyncLooper>(
                std::bind(&AsyncLogger::realLog, 
                          this, 
                          std::placeholders::_1), 
                asynctype
            ))
            {}
        private:
            void log(const char* data,size_t len)
            {
                _looper->push(data,len);

            }
            void realLog(Buffer& data)
            {
                if(_sinks.empty()) return;
                for(auto& sink:_sinks)
                {
                    sink->log(data.begin(),data.readAbleSize());
                }
            }
        private:
            AsyncLooper::ptr _looper;
    };

}