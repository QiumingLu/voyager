Voyager（旅行者）是一个C++多线程非阻塞网络库，它可以运行在Linux，Mac OS X, FreeBSD等类Unix操作系统上。该网络库采用Reactor模式，IO模型为IO multiplexing + non-blocking IO, 线程模型为one loop per thread + threadpool，每个线程最多有一个事件轮询，每个TCP连接都必须归某一个线程来管理，它所有的IO都在该线程上完成。

Voyager网络库的主要功能包括网络IO事件，定时任务管理，线程库和日志处理等。网络IO为非阻塞IO，基于事件的驱动和回调, 支持IO多路复用技术select，poll(2)，epoll(4)， kqueue。Voyager实现了TCP连接，并没有实现UDP连接。

Voyager采用C++11语言来编写，使用智能指针，RAII手法来管理内存，使用右值语义和移动语义来减少不必要的内存拷贝，使用function + bind来做事件的回调处理，使用lambda表达式来替换传统的函数指针等等。Voyager采用Posix线程，而非C++11标准库的线程，除此，还使用了部分GCC的基础设施，如私有线程储存等等。

Voyager的核心代码位于voyager目录中，其结构分为五部分，分别为util，port和core，http，paxos, docs。util为基础库，主要是实现了一些基础工具类，port主要是对Posix线程的封装，core为核心库，实现了网络库的核心功能， http为一个简单的http服务器，paxos为一个基于Leslie Lamport的Paxos协议实现的状态机。docs主要是一些文档说明。除此，网络库还实现了一些示例(示例代码位于examples目录中)，如suduku服务器和客户端的实现，echo示例。

对Voyager网络库做了很多对比测试（测试代码位于benchmarks目录中），Voyager在吞吐量和并发量方面表现优异。当然，Voyager并没有在安全性上做太多的处理，安全性和稳定性还有待加强。除此，还有很多细节方面需要完善。

编译环境：
（1）Linux 和 GCC 4.8
（2）Mac OS X 和 Clang 3.3

编译安装方法：

(1) LevelDB编译
Voyager的编译流程是基于leveldb1.19版本，如果你自行下载了其他版本，可能有编译上的不同。

进入third_party/leveldb目录。
make，编译完成后会在当前目录生成out-static/libleveldb.a文件。
mkdir lib建立一个lib目录，然后cd lib;ln -s ../out-static/libleveldb.a libleveldb.a建立一个软链，PhxPaxos通过lib这个目录来寻址静态库。

(2) Voyager编译
在voyager的根目录下，执行 sh build.sh
即可完成安装，安装后的目录为../build, 相关的测试及使用demo的执行文件在../build/release或../build/debug目录下，生产的lib库和所需的头文件在../build/release-install或../build/debug-install目录下。
