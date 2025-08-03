#pragma once

#include "AsyncBuffer.hpp"

#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <functional>
#include <memory>

namespace msylog
{
    enum class AsyncType
    {
        ASYNC_SAFE, //满了阻塞，避免资源耗尽
        ASYNC_UNSAFE //无限扩容
    };
    class AsyncLooper
    {
        public:
            using ptr = std::shared_ptr<AsyncLooper>;
            using Functor = std::function<void(Buffer&)>;
            AsyncLooper(const Functor& callback,AsyncType asynctype = AsyncType::ASYNC_SAFE)
            :_stop(false)
            ,_thread(std::thread(&AsyncLooper::threadEntry,this))
            ,_callback(callback)
            ,_asynctype(asynctype)
            {
            }
            ~AsyncLooper()
            {
                stop();
            }

            void flush()
            {
                std::unique_lock<std::mutex> lock(_mutex);
                if(!_pro_buf.empty()) {
                    _con_buf.swap(_pro_buf);
                    lock.unlock();
                    _callback(_con_buf);
                    _con_buf.reset();
                }
            }

            void stop()
            {
                _stop = true;
                _cond_con.notify_all();//唤醒所有的工作线程
                flush(); // 确保处理完剩余日志
                _thread.join();//等待工作线程的退出
            }
            void push(const char* data,size_t len)
            {
                //1.无限扩容 2.固定大小
                std::unique_lock<std::mutex> lock(_mutex);

                //我们在这里控制无限扩容或者固定大小，这段代码注释掉就是无限扩容，否则就是固定大小
                if(_asynctype == AsyncType::ASYNC_SAFE)
                    {_cond_pro.wait(lock,[&](){return _pro_buf.writeAbleSize()>=len;});}
                //能够走下来代表满足了条件，可以向缓冲区添加数据
                _pro_buf.push(data,len);
                //唤醒消费者对缓冲区中的数据进行处理
                _cond_con.notify_one();
            }

            //对消费缓冲区中的数据进行处理，处理完毕后，初始化缓冲区，交换缓冲区
            void threadEntry()
            {
                while(!_stop)
                {
                    //1.判断生产缓冲区有没有数据，有则交换，无则阻塞
                    {
                        std::unique_lock<std::mutex> lock(_mutex);
                        _cond_con.wait(lock, [&]()
                                       { return !_pro_buf.empty()||_stop;});
                        _con_buf.swap(_pro_buf);
                        //2.唤醒生产者 是安全模式就唤醒，不是安全模式直接不用唤醒了，因为不会阻塞
                        if(_asynctype == AsyncType::ASYNC_SAFE)
                            _cond_pro.notify_all();
                    }
                    //3.被唤醒后，对消费缓冲区进行数据处理
                    _callback(_con_buf);
                    //4.初始化消费缓冲区
                    _con_buf.reset();

                }
            }
        private:
            std::atomic<bool> _stop;
            Buffer _pro_buf;//生产缓冲区
            Buffer _con_buf;//消费缓冲区
            std::mutex _mutex;
            std::condition_variable _cond_pro;
            std::condition_variable _cond_con;
            std::thread _thread;//异步工作器对应的工作线程
            Functor _callback;
            AsyncType _asynctype;
    };
}
