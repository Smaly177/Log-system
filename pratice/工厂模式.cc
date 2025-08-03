// #include "stdio.h"
// #include "stdarg.h"
// //不定参宏函数
// #define LOG(fmt,...)printf("[%s:%d]" fmt,__FILE__,__LINE__,##__VA_ARGS__);

// //c语言中 不定参宏函数的使用，不定参数据的访问
// void printNum(int count,...)
// {
//     va_list ap;
//     va_start(ap,count);
//     for(int i = 0;i<count;i++)
//     {
//         int num = va_arg(ap,int);
//         printf("%d\n",num);
//     }
//     va_end(ap);
// }
// void myprintf(const char* fmt,...)
// {
//     va_list ap;
//     va_start(ap,fmt);

// }
// int main()
// {
//     // LOG("haha");
//     printNum(2,666,777);
//     printNum(3,1,2,3);
//     return 0;
// }



//简单工厂模式
// #include <iostream>
// #include <memory>
// #include <string>
// using namespace std;

// class Fruit
// {
// public:
//     virtual void name() = 0;
// };
// class Apple : public Fruit
// {
// public:
//     void name() override
//     {
//         cout<<"apple"<<endl;
//     }
// };
// class Banana : public Fruit
// {
//     public:
//     void name() override
//     {
//         cout<<"banana"<<endl;
//     }
// };
// class FruitFactory
// {
// public:
//     static shared_ptr<Fruit> create(const string& name)
//     {
//         if(name=="苹果")
//         {
//             return make_shared<Apple>();
//         }
//         else if(name=="香蕉")
//         {
//             return make_shared<Banana>();
//         }
//     }
// };
// int main()
// {

//     FruitFactory factory;
//     shared_ptr<Fruit> ptr = factory.create("苹果");
//     ptr->name();
//     ptr = factory.create("香蕉");
//     ptr->name();
//     /*
//         优点：简单
//         缺点：如果需要扩展新的产品，需要修改工厂类的东西，臃肿
//              我们的原则是对扩展开放，对修改关闭，不要修改原有内容
//     */
//     return 0;
// }



//为了解决修改工厂类的东西，提出了新的办法
//工厂方法模式
//每个产品对应一个工厂，建造多个工厂，就不用修改原有工厂了，直接新增
//一个工厂设置成抽象类，然后逐次建造子工厂，还是多态形式

#include <iostream>
#include <string>
#include <memory>
using namespace std;

class Fruit
{
public:
    virtual void name() = 0;
};
class Apple:public Fruit
{
public:
    void name() override
    {
        cout<<"苹果"<<endl;
    }
};
class Banana:public Fruit
{
public:
    void name() override
    {
        cout<<"香蕉"<<endl;
    }
};
class Pair:public Fruit
{
public:
    void name() override
    {
        cout<<"梨子"<<endl;
    }
};
class FruitFactory
{
public:
    virtual shared_ptr<Fruit> create() = 0;
};
class AppleFactory:public FruitFactory
{
public:
    shared_ptr<Fruit> create() override
    {
        return make_shared<Apple>();
    }
};
class BananaFactory:public FruitFactory
{
public:
    shared_ptr<Fruit> create() override
    {
        return make_shared<Banana>();
    }
};
class PairFactory:public FruitFactory
{
public:
    shared_ptr<Fruit> create() override
    {
        return make_shared<Pair>();
    }
};
int main()
{
    AppleFactory apple;
    BananaFactory banana;
    PairFactory pair;
    shared_ptr<Fruit> ptr = apple.create();
    ptr->name();
    ptr = banana.create();
    ptr->name();
    ptr = pair.create();
    ptr->name();
    /*
        优点：符合扩展原则
        缺点：类多，代码臃肿
    */
}
