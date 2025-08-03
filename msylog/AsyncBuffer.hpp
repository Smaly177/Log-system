#pragma once

/*
    异步日志缓冲区
    缓冲区设计
    设计一个缓冲区,直接存放格式化后的消息字符串
    好处:
        减少了LogMessage对象频繁的构造的消耗
        可以针对缓冲区中的日志消息一次性进行IO操作,减少IO次数,提高效率

    缓冲区类的设计:
        管理一个存放字符串数据的缓冲区 使用vector进行空间管理,不用string是为了防止\0也有
        写入数据位置的指针,指向可写区域起始位置
        读取数据位置的指针,指向可读区域起始位置
        当两个指针碰头代表数据取完了

*/

#include "LogUtil.hpp"

#include <vector>
#include<cassert>

namespace msylog
{
    #define DEFAULT_BUFFER_SIZE (1*1024*1024)
    #define THRESHOLD_BUFFER_SIZE (80*1024*1204) //小于此值,翻倍扩容,否则线性扩容
    #define INCREMENT_BUFFER_SIZE (10*1024*1024) //线程扩容值
    class Buffer
    {
        public:
            Buffer()
            :_buffer(DEFAULT_BUFFER_SIZE)
            {

            }
            //向缓冲区写入数据 
            void push(const char* data,size_t len)
            {
                //缓冲区剩余空间不够
                    //1.扩容 2.阻塞/返回false
                        //性能测试无限扩容,否则阻塞
                //1.固定大小,满了返回
                // if(len>writeAbleSize())return;
                //2.动态扩容,用于极限性能测试
                ensureEnoughSize(len);
                //将数据拷贝进缓冲区
                std::copy(data,data+len,&_buffer[_writer_idx]);
                //将当前写入位置向后偏移
                moveWriter(len);

            }
            //对读写指针进行向后偏移操作
            void moveWriter(size_t len)
            {
                assert((len+_writer_idx)<=_buffer.size());
                _writer_idx += len;
            }
            void moveReader(size_t len)
            {
                assert(len<=readAbleSize());
                _reader_idx += len;
            }
            //返回可读数据起始地址
            const char* begin()
            {
                return &_buffer[_reader_idx]; 
            }
            //返回缓冲区大小
            size_t readAbleSize()
            {
                //因为当前实现的缓冲区并不是循环缓冲区,因此只会向后写,不存在空间循环使用的情况
                return (_writer_idx-_reader_idx);
            }
            size_t writeAbleSize()
            {
                //对于扩容思路来说,不存在可写空间大小,总是可写
                //因此这个接口仅仅针对提供固定大小缓冲区的接口
                return (_buffer.size()-_writer_idx);
                
            }

            //重置
            void reset()
            {
                _reader_idx = 0; //缓冲区所有空间都是空闲的
                _writer_idx = 0; //表示没有数据可读
            }
            
            //两个缓冲区交换操作
            void swap(Buffer& buffer)
            {
                _buffer.swap(buffer._buffer);
                std::swap(_reader_idx,buffer._reader_idx);
                std::swap(_writer_idx,buffer._writer_idx) ;
            }

            bool empty()
            {
                return (_reader_idx == _writer_idx);
            }
            void ensureEnoughSize(size_t len)
            {

                if(len<writeAbleSize()) return;
                size_t new_size = 0;
                if(_buffer.size()<THRESHOLD_BUFFER_SIZE)
                {
                    new_size = _buffer.size()*2+len; //小于阈值则翻倍增长
                }
                else
                {
                    new_size =  _buffer.size() + INCREMENT_BUFFER_SIZE;//否则线性增长
                }
                _buffer.resize(new_size);
            }
        private:
            std::vector<char> _buffer;
            size_t _reader_idx = 0;
            size_t _writer_idx = 0;
    };

}