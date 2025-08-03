/*

    最终性能测试模块

    100w条指定长度数据
    每秒可以输出多少条日志
    每秒可以输出多少M日志 1M=1024*1024 


*/


#include "../msylog/MsyLog.h"

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <numeric>
using namespace std;


void test(string logger_name,size_t threadNum,size_t message_count,size_t message_len)
{

    cout<<"日志数量: "<<message_count<<"条"<<endl;
    cout<<"每个日志长度字节: "<<message_len<<endl;
    cout<<"日志总大小: "<<message_count*message_len/1024<<"Kb"<<endl;
    //前置信息
    msylog::Logger::ptr logger = getLogger(logger_name); //日志器
    string msg(message_len-1,'A');//日志信息 少一个字节 给\n
    vector<thread> threads;//线程
    vector<double> cost_array(threadNum); //耗时
    size_t per_thread_message_count = message_count/threadNum; //每个线程负责日志数量
    
    //启动所有线程
    for(int i = 0;i<threadNum;i++)
    {
        threads.emplace_back([&,i]()
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int j = 0;j<per_thread_message_count;j++)
        {
            logger->fatal("%s",msg.c_str());
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> cost = end-start;
        cost_array[i] = cost.count();
        cout<<"线程"<<i<<"\t输出日志数量: "<<per_thread_message_count<<",耗时: "<<cost.count()<<"s"<<endl;
    }
                            );
    }
    //结束线程
    for(int i = 0;i<threadNum;i++)
    {
        threads[i].join();
    }
    //计算耗时
    double max_cost = cost_array[0];
    for(int i = 0;i<threadNum;i++)
    {
        max_cost = std::max(max_cost,cost_array[i]);
    }
    //每秒输出日志数量
    size_t per_sec_message = message_count/max_cost;
    //每秒输出日志大小
    size_t per_sec_count = message_count * message_len/ (max_cost*1024);
    //输出结果
    cout<<"每秒输出日志数量："<<per_sec_message<<"条"<<endl;
    cout<<"每秒输出日志大小："<<per_sec_count<<"Kb"<<endl;
}
void testsync(int threadNum)
{
    std::unique_ptr<msylog::LoggerBuilder> log (new msylog::GlobalLoggerBuilder());
    log->buildLoggerName("同步日志器");
    log->buildLoggerType(msylog::LoggerType::LOGGER_SYNC);
    log->buildSink<msylog::FileSink>("./file/file.log");
    log->buildLoggerFormatter("%m%n");
    log->build();
    test("同步日志器",threadNum,1000000,100);  
}
void testasync(int threadNum)
{
    std::unique_ptr<msylog::LoggerBuilder> log (new msylog::GlobalLoggerBuilder());
    log->buildLoggerName("异步日志器");
    log->buildLoggerType(msylog::LoggerType::LOGGER_ASYNC);
    log->buildSink<msylog::FileSink>("./file/file.log");
    log->buildLoggerFormatter("%m%n");
    log->buildAsyncTypeUnSafe();
    log->build();
    test("异步日志器",threadNum,1000000,100);  
}

int main()
{

    //  testsync(1);
// 日志数量: 1000000条
// 每个日志长度字节: 100
// 日志总大小: 97656Kb
// 线程0   输出日志数量: 1000000,耗时: 2.35701s
// 每秒输出日志数量：424266条
// 每秒输出日志大小：41432Kb 

    //  testsync(3);
// 日志数量: 1000000条
// 每个日志长度字节: 100
// 日志总大小: 97656Kb
// 线程2   输出日志数量: 333333,耗时: 2.57063s
// 线程1   输出日志数量: 333333,耗时: 2.59196s
// 线程0   输出日志数量: 333333,耗时: 2.6233s
// 每秒输出日志数量：381199条
// 每秒输出日志大小：37226Kb //设计到锁冲突

    // testasync(1);
// 日志数量: 1000000条
// 每个日志长度字节: 100
// 日志总大小: 97656Kb
// 线程0   输出日志数量: 1000000,耗时: 2.01498s
// 每秒输出日志数量：496282条
// 每秒输出日志大小：48465Kb

    // testasync(3);
// 日志数量: 1000000条
// 每个日志长度字节: 100
// 日志总大小: 97656Kb
// 线程2   输出日志数量: 333333,耗时: 1.55494s
// 线程0   输出日志数量: 333333,耗时: 1.55611s
// 线程1   输出日志数量: 333333,耗时: 1.56962s
// 每秒输出日志数量：637095条
// 每秒输出日志大小：62216Kb
}

//测试结果差距不大是因为我们的测试环境没有业务，同步跟异步差不多，如果真实开发场景，大量的业务以及网络IO
//中 异步远超于同步
/*
    同步主要取决于磁盘，并且 多线程存在锁冲突
    异步主要取决于cpu和内存性能
*/