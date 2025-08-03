#pragma once


#include "LogLevel.hpp"
#include "LogMessage.hpp"

#include <sstream>
#include <ctime>
#include <memory>
#include <vector>
#include <cassert>

 namespace msylog
 {
    class FormatItem
    {
        public:
            using ptr = std::shared_ptr<FormatItem>;
            virtual void format(std::ostream& out,LogMessage& message) = 0;
    };
    class MessageItem:public FormatItem
    {
        public:
            void format(std::ostream& out,LogMessage& message) override
            {
                out<<message._payload;
            }
    };
    class LevelItem:public FormatItem
    {
        public:
            void format(std::ostream& out,LogMessage& message) override
            {
                out<<LogLevel::toString(message._level);
            }
    };
    class TimeItem:public FormatItem
    {
        public:
            TimeItem(const std::string& fmt = "%Y-%m-%d %H:%M:%S"):_time_fmt(fmt){}
            void format(std::ostream& out,LogMessage& message) override
            {
                struct tm t;
                localtime_r(&message._ctime,&t);
                char tmp[32] = {0};
                strftime(tmp,31,_time_fmt.c_str(),&t);
                out<<tmp;
            }
        private:
            std::string _time_fmt;//%H:%M:%S
    };
    class FileItem:public FormatItem
    {
        public:
            void format(std::ostream& out,LogMessage& message) override
            {
                out<<message._file;
            }
    };
    class LineItem:public FormatItem
    {
        public:
            void format(std::ostream& out,LogMessage& message) override
            {
                out<<message._line;
            }
    };
    class ThreadItem:public FormatItem
    {
        public:
            void format(std::ostream& out,LogMessage& message) override
            {
                out<<message._tid;
            }
    };
    class TabItem:public FormatItem
    {
        public:
            void format(std::ostream& out,LogMessage& message) override
            {
                out<<"\t";
            }
    };
    class NLineItem:public FormatItem
    {
        public:
            void format(std::ostream& out,LogMessage& message) override
            {
                out<<"\n";
            }
    };
    class LoggerItem:public FormatItem
    {
        public:
            void format(std::ostream& out,LogMessage& message) override
            {
                out<<message._logger;
            }
    };
    class OtherItem:public FormatItem
    {
    public:
        OtherItem(const std::string& str):_str(str){}
        void format(std::ostream& out,LogMessage& message) override
        {
            out << _str;  // 正确：输出存储的原始字符（如方括号）
        }
    private:
        std::string _str;  // 存储原始字符（方括号、空格等）
    };

    //真正的格式化器
    class Formatter
    {
        public:
            using ptr = std::shared_ptr<Formatter>;
            Formatter(const std::string& pattern = "[%d{%Y-%m-%d %H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n")
            :_pattern(pattern)
            {
                assert(parsePattern()); 
            }
            void format(std::ostream& out,LogMessage& message)
            {
                for(auto& item:_items)
                {
                    //这里的format是上面的子类的虚函数,注意!不是下面的重载
                    item->format(out,message);
                } 
            }
            std::string format(LogMessage& message) //对原始消息进行格式化生成要的消息
            {
                std::stringstream ss;
                format(ss,message);
                return ss.str();
            }

            bool parsePattern()
            {
                // return 1 ;//TODO...................................................
                // 1.对格式化规则字符串进行解析
                // 2.根据解析得到的数据初始化格式化子项数组成员_items
                // aaaa[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n

                // 解析
                std::vector<std::pair<std::string, std::string>> fmt_order;
                size_t pos = 0;
                std::string key, val;
                while (pos < _pattern.size())
                {
                    // 处理原始字符串 最前面的
                    // 处理原始字符串(包括方括号等)
                    if (_pattern[pos] != '%')
                    {
                        fmt_order.push_back(std::make_pair("", std::string(1, _pattern[pos++])));
                        continue;
                    }
                    // 能走下来就代表找到%了
                    // 再判断是不是双%%的情况
                    if (pos + 1 < _pattern.size() && _pattern[pos + 1] == '%')
                    {
                        val.push_back('%');
                        pos += 2;
                        continue;
                    }
                    if (val.empty() == false)
                    {
                        fmt_order.push_back(std::make_pair("", val));
                        val.clear();
                    }
                    //----------------------
                    // 这时候是格式化字符的处理
                    // 能走这里代表就是格式字符了
                    pos += 1; // 取出%后的格式化字符
                    if (pos == _pattern.size())
                    {
                        std::cout << "%之后没有对应的格式化字符!\n";
                        return false;
                    }
                    key = _pattern[pos];
                    // 再判断有没有花括号，时间有花括号 字串
                    pos += 1;

                    if (pos < _pattern.size() && _pattern[pos] == '{')
                    {
                        pos += 1;

                        while (pos < _pattern.size() && _pattern[pos] != '}')
                        {
                            val.push_back(_pattern[pos++]);
                        }
                        // 走到了末尾跳出循环 没找到} 格式错误 退出
                        if (pos == _pattern.size())
                        {
                            std::cout << "子规则{}匹配出错\n";
                            return false; // 没有找到}的另一半
                        }

                        // 否则找到},继续执行
                        pos += 1; // 因为这时候Pos指向的是}位置，向后走一步，走到了下次处理的新位置
                    }
                    fmt_order.push_back(std::make_pair(key, val));
                    key.clear();
                    val.clear();
                }

                // 根据解析数据初始化子项数组成员
                for (auto &it : fmt_order)
                {
                    _items.push_back(createItem(it.first, it.second));
                }
                return true;
            }

        private:
            //根据不同的格式化字符创建不同的格式化子项对象
            FormatItem::ptr createItem(const std::string& key,const std::string& val)
            {
                if(key == "d") return std::make_shared<TimeItem>(val);
                if(key == "t") return std::make_shared<ThreadItem>();
                if(key == "c") return std::make_shared<LoggerItem>();
                if(key == "f") return std::make_shared<FileItem>();
                if(key == "l") return std::make_shared<LineItem>();
                if(key == "p") return std::make_shared<LevelItem>();
                if(key == "T") return std::make_shared<TabItem>();
                if(key == "m") return std::make_shared<MessageItem>();
                if(key == "n") return std::make_shared<NLineItem>();
                return std::make_shared<OtherItem>(val);
            }

            std::string _pattern; //格式化规则字符串
            std::vector<FormatItem::ptr> _items;
    };
 }
