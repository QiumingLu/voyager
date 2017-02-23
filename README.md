Voyager（旅行者）是一个C++多线程非阻塞网络库，它可以运行在Linux，Mac OS X, FreeBSD等类Unix操作系统上。该网络库采用Reactor模式，IO模型为IO multiplexing + non-blocking IO, 线程模型为one loop per thread + threadpool，每个线程最多有一个事件轮询，每个TCP连接都必须归某一个线程来管理，它所有的IO都在该线程上完成。

Voyager网络库的主要功能包括网络IO事件，定时任务管理，线程库和日志处理等。网络IO为非阻塞IO，基于事件的驱动和回调, 支持IO多路复用技术select，poll(2)，epoll(4)， kqueue。Voyager实现了TCP连接，并没有实现UDP连接。

Voyager采用C++11语言来编写，使用智能指针，RAII手法来管理内存，使用右值语义和移动语义来减少不必要的内存拷贝，使用function + bind来做事件的回调处理，使用lambda表达式来替换传统的函数指针等等。Voyager采用Posix线程，而非C++11标准库的线程，除此，还使用了部分GCC的基础设施，如私有线程储存等等。

Voyager的核心代码位于voyager目录中，其结构分为六部分，分别为util，port，core，http, rpc和docs。util为基础库，主要是实现了一些基础工具类，port主要是对Posix线程的封装，core为核心库，实现了网络库的核心功能， http为一个简单的http服务器，rpc为在Google Protobuf基础上搭建的一个rpc库，docs主要是一些文档说明。除此，网络库还实现了一些示例(示例代码位于examples目录中)，如suduku服务器和客户端的实现，echo示例。

对Voyager网络库做了很多对比测试（测试代码位于benchmarks目录中），Voyager在吞吐量和并发量方面表现优异。当然，Voyager并没有在安全性上做太多的处理，安全性和稳定性还有待加强。除此，还有很多细节方面需要完善。

编译环境：
（1）Linux, GCC 4.8 和 CMake 3.0
（2）Mac OS X, Clang 3.3 和 CMake3.0

编译安装方法：

在voyager的根目录下，执行 ./build.sh
即可完成编译，编译后生成的目录为./build/release, 相关的测试及使用demo的执行文件在./build/release目录下。
到./build/release目录下，执行sudo make install,即可完成安装，默认安装目录为/usr/local(可通过修改build.sh文件来修改安装选项)。
