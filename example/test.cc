#include "LogUtil.hpp"
#include "LogLevel.hpp"
#include "LogMessage.hpp"
#include "LogFormat.hpp"
#include "LogSink.hpp"
#include "LogLogger.hpp"
#include "LogBuilderModel.hpp"
#include "AsyncBuffer.hpp"
#include "AsyncLooper.hpp"
#include "MsyLog.h"
// int main()
// {
// //测试获取时间
//     // std::cout<<msylog::util::Date::getTime();
// //测试文件工具类
//     // std::string pathname = "./a/b/c/1.txt";
//     // std::cout<<msylog::util::File::getFilePath(pathname)<<std::endl;
//     // if(msylog::util::File::exists(pathname)) std::cout<<"存在此文件"<<std::endl;
//     // msylog::util::File::createDirectory(msylog::util::File::getFilePath(pathname));
//     // if(msylog::util::File::exists(pathname)) std::cout<<"存在此文件"<<std::endl;
// //测试日志等级类
//     // std::cout<<msylog::LogLevel::toString(msylog::LogLevel::value::DEBUG)<<std::endl;
//     // std::cout<<msylog::LogLevel::toString(msylog::LogLevel::value::WARN)<<std::endl;
//     // std::cout<<msylog::LogLevel::toString(msylog::LogLevel::value::INFO)<<std::endl;
//     // std::cout<<msylog::LogLevel::toString(msylog::LogLevel::value::ERROR)<<std::endl;
//     // std::cout<<msylog::LogLevel::toString(msylog::LogLevel::value::FATAL)<<std::endl;
//     // std::cout<<msylog::LogLevel::toString(msylog::LogLevel::value::UNKNOW)<<std::endl;
// //测试格式化器类
//     // msylog::LogMessage message(msylog::LogLevel::value::INFO,53,"main.cc","root","格式化功能测试消息...");
//     // msylog::Formatter fmt;
//     // // std::string str = fmt.format(message);
//     // // std::cout<<str<<std::endl;
//     // // msylog::Formatter fmt("[%d{%H:%M:%S}]%m");
//     // std::string str = fmt.format(message);
//     // std::cout<<str<<std::endl;
// //测试落地方式以及工厂模式参数包的使用
//     // msylog::LogMessage message(msylog::LogLevel::value::INFO,53,"main.cc","root","落地消息功能测试...");
//     // msylog::Formatter fmt;
//     // std::string str = fmt.format(message);

//     // msylog::LogSink::ptr stdoutptr = msylog::SinkFactory::create<msylog::StdoutSink>();

//     // msylog::LogSink::ptr fileptr = msylog::SinkFactory::create<msylog::FileSink>("./file/log.txt");

//     // msylog::LogSink::ptr rollptr = msylog::SinkFactory::create<msylog::RollBySizeSink>("./roll/log-",1024*1024);    

//     // for(int i = 1;i<=50000;i++)
//     // {
//     //     std::string message;
//     //     message += std::to_string(i);
//     //     message += str;

//     //     stdoutptr->log(message.c_str(),message.size());
//     //     fileptr->log(message.c_str(),message.size());
//     //     rollptr->log(message.c_str(),message.size());
//     // }

//     // size_t cursize = 0;
//     // while(cursize<1024*1024*10)
//     // {
//     //     rollptr->log(str.c_str(),str.size());
//     //     cursize+=str.size();
//     // }

//     // return 0;
// }

    
    //测试支持扩展功能
    //扩展一个按时间进行滚动
// enum class TimeGap
// {
//     GAP_SECOND,
//     GAP_MINUTE,
//     GAP_HOUR,
//     GAP_DAY
// };
// class RollByTimeSink : public msylog::LogSink
// {
//     public:
//         RollByTimeSink(const std::string& basename,TimeGap gap_type)
//         :_basename(basename)
//         {
//             switch(gap_type)
//             {
//                 case TimeGap::GAP_SECOND:_gap_size = 1;break;
//                 case TimeGap::GAP_HOUR:_gap_size = 3600;break;
//                 case TimeGap::GAP_MINUTE:_gap_size = 60;break;
//                 case TimeGap::GAP_DAY:_gap_size = 3600*24;break;
//             }
//             time_t now = msylog::util::Date::getTime();
//             _cur_gap = _gap_size == 1 ? now : now / _gap_size; // 计算当前时间段
//             std::string filename = createNewFile();
//             msylog::util::File::createDirectory(msylog::util::File::getFilePath(filename));
//             _ofs.open(filename,std::ios::binary | std::ios::app);
//             assert(_ofs.is_open());
//         }
//         void log(const char* data,size_t len)
//         {
//             time_t cur = msylog::util::Date::getTime();
//             time_t new_gap = _gap_size == 1 ? cur : cur / _gap_size;
//             if(new_gap != _cur_gap)
//             {
//                 _cur_gap = new_gap;
//                 _ofs.close();
//                 std::string filename = createNewFile();
//                 _ofs.open(filename, std::ios::binary|std::ios::app);
//                 assert(_ofs.is_open());
//             }
//             _ofs.write(data,len);
//             assert(_ofs.good());
//         }
//         std::string createNewFile()
//         {
//             time_t t = msylog::util::Date::getTime();
//             struct tm lt;
//             localtime_r(&t,&lt);
//             std::stringstream filename;
//             filename<<_basename;
//             // 年份：加上1900
//             filename << (lt.tm_year + 1900);
//             // 月份：加上1，并且确保有两位数字
//             filename << std::setw(2) << std::setfill('0') << (lt.tm_mon + 1);
//             // 日期、小时、分钟、秒：确保都是两位数字
//             filename << std::setw(2) << std::setfill('0') << lt.tm_mday;
//             filename << std::setw(2) << std::setfill('0') << lt.tm_hour;
//             filename << std::setw(2) << std::setfill('0') << lt.tm_min;
//             filename << std::setw(2) << std::setfill('0') << lt.tm_sec;
//             filename<<".log";  // 移除count计数器
//             return filename.str();
//         }
//     private:
//         std::string _basename;
//         std::ofstream _ofs;
//         size_t _cur_gap; //当前是第几个时间段    
//         size_t _gap_size;//时间段的大小
// };
// int main()
// {
//     msylog::LogMessage message(msylog::LogLevel::value::INFO,53,"main.cc","root","扩展功能测试...");
//     msylog::Formatter format;
//     std::string str = format.format(message);
//     msylog::LogSink::ptr timeptr = msylog::SinkFactory::create<RollByTimeSink>("./roll/log-",TimeGap::GAP_SECOND);
    
//     time_t old = msylog::util::Date::getTime();
//     while(msylog::util::Date::getTime()<old+5)
//     {
//         timeptr->log(str.c_str(),str.size());
//         usleep(1000);
//     }
// }


//   // 测试LogLogger 整合器
// int main()
// {
//     msylog::Formatter::ptr formatter(new msylog::Formatter());
//     msylog::LogSink::ptr stdoutptr = msylog::SinkFactory::create<msylog::StdoutSink>();
//     msylog::LogSink::ptr fileptr = msylog::SinkFactory::create<msylog::FileSink>("./file/log.log");
//     msylog::LogSink::ptr rollbysizeptr = msylog::SinkFactory::create<msylog::RollBySizeSink>("./roll/log-",1024);
//     std::vector<msylog::LogSink::ptr> sinks;
//     sinks.push_back(stdoutptr);
//     // sinks.push_back(fileptr);
//     // sinks.push_back(rollbysizeptr);
//     msylog::SyncLogger synclogger("同步日志器",msylog::LogLevel::value::FATAL,formatter,sinks);
//     synclogger.info(__FILE__,__LINE__,"%s-%d","哈哈",1);
//     synclogger.debug(__FILE__,__LINE__,"%s-%d","哈哈",1);
//     synclogger.error(__FILE__,__LINE__,"%s-%d","哈哈",1);
//     synclogger.fatal(__FILE__,__LINE__,"%s-%d","哈哈",1);
//     synclogger.warn(__FILE__,__LINE__,"%s-%d","哈哈",1);
//     // size_t size = 1;
//     // while(size<=1000*5)
//     // {
//     //     synclogger.info(__FILE__,__LINE__,"%s-%d","哈哈",size);
//     //     size++;
//     // }
//     return 0;
// }



// //建造者模式测试 同步
// int main()
// {
//     std::unique_ptr<msylog::LoggerBuilder> builderptr(new msylog::LocalLoggerBuilder());
//     builderptr->buildLoggerName("同步日志器");
//     builderptr->buildLoggerType(msylog::LoggerType::LOGGER_SYNC);
//     builderptr->buildLoggerLevel(msylog::LogLevel::value::DEBUG);
//     builderptr->buildLoggerFormatter();
//     builderptr->buildSink<msylog::StdoutSink>();
//     builderptr->buildSink<msylog::FileSink>("./file/log.log");
//     builderptr->buildSink<msylog::RollBySizeSink>("./roll/log-",1024);

//     msylog::Logger::ptr logger = builderptr->build();
//     logger->debug(__FILE__,__LINE__,"%s-%d","建造者模式测试...",100);

//     return 0;
// }


//     //异步缓冲区测试
// int main()
// {
//     std::ifstream ifs("./LogLevel.hpp",std::ios::binary);
//     if(ifs.is_open()==false)return -1;
//     std::string body;
//     ifs.seekg(0,std::ios::end);
//     size_t fsize = ifs.tellg();
//     ifs.seekg(0,std::ios::beg);
//     body.resize(fsize);
//     ifs.read(&body[0],fsize);
//     if(ifs.good()==false)
//     {
//         std::cout<<"read error\n"; return -1;
//     } 
//     ifs.close();
//     msylog::Buffer buffer;
//     for(int i = 0;i<body.size();i++)
//     {
//         buffer.push(&body[i],1);
//     }
//     std::ofstream ofs("./blog.log",std::ios::binary);
//     // ofs.write(buffer.begin(),buffer.readAbleSize());
//     size_t rsize = buffer.readAbleSize();
//     for(int i = 0;i<rsize;i++)
//     {
//         ofs.write(buffer.begin(),1);
//         buffer.moveReader(1);
//     }
//     ofs.close();
//     return 0;
// }



//     //异步日志器建造者测试
// int main()
// {
//     // 日志器类型
//     msylog::LoggerType loggertype = msylog::LoggerType::LOGGER_ASYNC;
//     //名称
//     std::string name = "异步日志器";
//     //等级
//     msylog::LogLevel::value limit_level = msylog::LogLevel::value::DEBUG;
//     //落地方式
//     // msylog::LogSink::ptr sinkptr = msylog::SinkFactory::create<msylog::StdoutSink>();
//     // msylog::LogSink::ptr sinkptr = msylog::SinkFactory::create<msylog::StdoutSink>();
//     // msylog::LogSink::ptr sinkptr = msylog::SinkFactory::create<msylog::StdoutSink>();
//     std::unique_ptr<msylog::LoggerBuilder> asyncptr(new msylog::LocalLoggerBuilder());
//     asyncptr->buildLoggerName(name);
//     asyncptr->buildLoggerLevel(limit_level);
//     asyncptr->buildLoggerType(loggertype);
//     asyncptr->buildLoggerFormatter();
//     asyncptr->buildAsyncTypeUnSafe();
//     asyncptr->buildSink<msylog::StdoutSink>();
//     asyncptr->buildSink<msylog::FileSink>("./file/file.log");
//     asyncptr->buildSink<msylog::RollBySizeSink>("./roll/roll-",1024*1024);
//     msylog::Logger::ptr ptr = asyncptr->build();
//     int i = 1;
//     while(i<=100000)
//     {
//         ptr->info(__FILE__,__LINE__,"%s-->%d","哈哈",i);
//         i++;
//     }
//     // sleep(10);
//      //异步情况下 按文件滚动有问题
// }


// //测试单例管理类
// int main()
// {
//     // msylog::Logger::ptr logger = msylog::LoggerManager::getInstance().getRootLogger();
//     // logger->info(__FILE__,__LINE__,"HHHWHW");
//     // logger->debug(__FILE__,__LINE__,"HHHWHW");
//     std::unique_ptr<msylog::LoggerBuilder> builder(new msylog::GlobalLoggerBuilder);
//     builder->buildLoggerName("同步日志器");
//     builder->buildLoggerType(msylog::LoggerType::LOGGER_SYNC);
//     builder->buildLoggerLevel(msylog::LogLevel::value::DEBUG);
//     builder->buildLoggerFormatter();
//     builder->buildSink<msylog::StdoutSink>();
//     builder->buildSink<msylog::FileSink>("./file/file.log");
//     builder->build();

//     msylog::Logger::ptr log = msylog::LoggerManager::getInstance().getLogger("同步日志器");
//     for(int i = 1;i<=400;i++)
//     {
//         log->info(__FILE__,__LINE__,"%s-%d","睡..",i);
//     }
//     return 0;
// }



//测试MsyLog.hpp文件 全局代理 方便用户使用 设置了宏函数

// int main()
// {
//     std::unique_ptr<msylog::LoggerBuilder> builder(new msylog::GlobalLoggerBuilder);
//     builder->buildLoggerName("同步日志器");
//     builder->buildLoggerType(msylog::LoggerType::LOGGER_SYNC);
//     builder->buildLoggerLevel(msylog::LogLevel::value::DEBUG);
//     builder->buildLoggerFormatter();
//     builder->buildSink<msylog::StdoutSink>();
//     builder->buildSink<msylog::FileSink>("./file/file.log");
//     builder->build();
    
//     msylog::Logger::ptr ptr = getLogger("同步日志器");

//     for(int i = 1;i<=50;i++)
//     {
//         INFO("%s--%d","哈哈",i);
//     }
//     for(int i = 51;i<=100;i++)
//     {
//         ptr->debug("哈哈-%d",i);
//     }
// }