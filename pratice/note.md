cout 输出 字符指针char* 以\0终止 
cout按十进制
cout.write按二进制,可指定字节数,不以\0终止

cout<<是ostream类里的重载
cout.write是ostream里的成员函数

c++ 三种文件操作方式
c++标准库 fstream 最常用 _ofs.close();//注意关闭之前的文件
c fopen
c++17文件库

在 C++ 中，对于父类中的虚函数（非纯虚函数），如果不声明为纯虚函数（即不写 =0），那么父类必须提供该函数的定义，否则会导致链接错误（如你之前遇到的 undefined reference to vtable for msylog::Logger）。

异步日志系统中缓冲期的设计
不能用链表,会涉及到频繁的申请空间与释放,性能降低
使用环形队列,双指针,一个指针写,一个指针读,但是存在大量的锁冲突,所以使用双缓冲区来解决
双缓冲区,工作线程写日志到写入缓冲区,此时异步线程不参与,没有了锁冲突
等工作线程写满后将写入缓冲区内容放入处理缓冲区,异步线程就工作,互不冲突,两个线程间的冲突解决了
只有在两个缓冲区进行交换的时候才会产生一次锁冲突

缓冲区
无缓冲 直接写入目标设备 write()
行缓冲  stdout标准输出 遇到\n才刷新到目标设备
全缓冲  缓冲区满了才刷新,可手动fflush刷新

写入缓冲区与内存打交道 速度极快,适用于频繁操作 如日志 批量处理等
打印到控制台 标准输出stdout 默认行缓冲或无缓冲 
    设计到内核态切换,速度慢,如果终端显示速度慢,可能造成阻塞
    适用于调试信息 实时交互输出 性能最慢
写入文件 默认全缓冲
    数据先写入内核缓冲区,再由操作系统异步刷盘
    适用于持久化存储,性能中等,受磁盘类型hdd ssd影响大

内存写入（缓冲）	内存寻址、数据拷贝	纳秒级（ns）
控制台输出	系统调用、终端渲染	毫秒级（ms）
文件写入（磁盘）	系统调用、文件系统操作、磁盘寻道	毫秒级（ms）~ 秒级（s）
系统调用（如 write()、printf()）涉及用户态与内核态切换，耗时远高于内存操作。
磁盘 I/O 受物理限制（如 HDD 的机械寻道时间），比内存操作慢约 5-6 个数量级。

优化建议
减少控制台输出
避免在循环或高频函数中打印大量数据。
使用条件编译（如 #ifdef DEBUG）控制调试信息输出。
利用缓冲区提高文件写入性能

使用 setvbuf() 调整缓冲区大小（默认 8KB）
FILE* fp = fopen("data.txt", "w");
setvbuf(fp, nullptr, _IOFBF, 64 * 1024);  // 设置 64KB 全缓冲

批量写入数据，减少系统调用次数：
std::string buffer;
for (...) {
    buffer += generateData();  // 先积累数据到缓冲区
}
fwrite(buffer.data(), 1, buffer.size(), fp);  // 一次性写入

异步 I/O
对于高并发场景，使用异步方式处理文件或网络 I/O：
#include <future>

// 异步写入文件
auto future = std::async(std::launch::async, [&]() {
    writeToFile(data);
});

典型应用场景
高性能日志系统：先写入内存缓冲区，再批量刷盘（如 spdlog、glog）。
实时监控系统：控制台输出用于调试，关键数据异步写入文件或数据库。
大数据处理：内存中聚合数据，最后一次性写入磁盘。

vector跟String vector可以\0 string不可以\0


vector常用的构造方式
    vector<int> v1;
    vector<int> v1(5); 5个元素 每个元素都是0初始值
    vector<int> v1(3,10) 3个10
    vector<int> v1 = {1,2,3} //c++11之后支持
    vector<int> v1{1,2,3} //c++11之后支持
    vector<int> v1(v2.begin(),v2.end())
    vector<int> v1(v2)
    vector<int> v1 = v2


STL细节:
    自带find的 一般都是关联式容器,对查找要求高,自带find成员函数
        set map multiset multimap unordered_set unordered_map unordered_multi... string
    没有find的 一般都是序列式容器,查找不是核心功能,没有内置find,需使用标准库中的std::find
        std::find原理线性遍历 On 
        template<class InputIt, class T>
        InputIt find(InputIt first, InputIt last, const T& value);
        vector list deque array forward_list
    迭代器失效:
        vector 插入 删除
        deque
        array
        list 删除 仅删除节点的迭代器失效 其他有效
        set/map/multi... 删除 仅删除节点的迭代器失效 其他有效
        哈希 插入未触发rehash有效 触发无效 删除 仅...



md5sum 文件名 可以生成唯一哈希标识符,两个文件标识符如果一样 代表内容完全一样




树
节点的度：一个节点含有的子树的个数成为该节点的度
叶子节点
分支节点
