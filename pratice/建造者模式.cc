#include <iostream>
#include <string>
#include <memory>
using namespace std;

class Computer
{
public:
    Computer(){}
    void setBoard(const string& name)
    {
        _board = name;
    }
    void setDisplay(const string& name)
    {
        _display = name;
    }
    virtual void setOs() = 0;
    void show()
    {
        cout<<"主板->"<<_board<<endl;
        cout<<"显示器->"<<_display<<endl;
        cout<<"操作系统->"<<_os<<endl;
    }
protected:
    string _board;//键盘
    string _display;//显示器
    string _os;//操作系统
};
class MacBook :public Computer
{
    public:
    void setOs() override
    {
        _os = "Mac Os 12";
    }
};
class Builder
{
    public:
        virtual void buildBoard(const std::string& board) = 0;
        virtual void buildDisplay(const std::string& display) = 0;
        virtual void buildOs() = 0;
        virtual shared_ptr<Computer> build() = 0;
};
class MacBuilder:public Builder
{
public:
    MacBuilder()
    :_computer(new MacBook())
    {    
    }
    void buildBoard(const string& board)
    {
        _computer->setBoard(board);
    }
    void buildDisplay(const string& display)
    {
        _computer->setDisplay(display);
    }
    void buildOs()
    {
        _computer->setOs();
    }
    shared_ptr<Computer> build()
    {
        return _computer;
    }

    shared_ptr<Computer> _computer;
};

class Director
{
public:
    Director(Builder* builder)
    :_builder(builder)
    {

    }
    void construct(const string& board,const string& display)
    {
        _builder->buildBoard(board);
        _builder->buildDisplay(display);
        _builder->buildOs(); 
    }
private:
    shared_ptr<Builder> _builder;
};
int main()
{

    Builder* builder = new MacBuilder();
    Director director(builder);
    director.construct("京东主板","三星显示器");

    shared_ptr<Computer> computer = builder->build();
    computer->show();
    return 0;
}