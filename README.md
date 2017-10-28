# Voyager -- C++多线程网络库
## 简介
Voyager（旅行者）是一个C++多线程网络库，可以运行在Linux，Mac OS X, FreeBSD等类Unix操作系统上。该网络库采用Reactor模式，IO模型为IO multiplexing + non-blocking IO, 线程模型为one loop per thread + threadpool，每个线程最多有一个事件轮询，每个TCP连接都必须归某一个线程来管理，它所有的IO都在该线程上完成。
<br/>
<br/>[![Build Status](https://travis-ci.org/QiumingLu/voyager.svg?branch=master)](https://travis-ci.org/QiumingLu/voyager)
<br/>
<br/>Author: Mirants Lu (mirantslu@gmail.com) 
<br/>
<br/>**Voyager主要包含以下几个部分**：
<br/>
<br/>1. **util**: 基础库模块，主要包含日志处理，字符串处理，一个简单的JSON程序和一些工具类。
<br/>2. **core**: 核心网络库，实现基本的网络传输功能和定时器等。
<br/>3. **http**: HTTP服务器，在网络库的基础上搭建的一个不完善的http服务器。
<br/>4. **rpc**:  RPC远程调用库，基于Voyager和Google Protobuf搭建的一个RPC库。
<br/>5. **docs**: 一些文档说明。
<br/>6. **examples**: Voyager的使用示例，如Sudoku服务器和客户端的实现。
<br/>7. **benchmarks**: 对比测试，借鉴于Boost的asio和libevent等的测试方法，来进行吞吐量和并发量的测试。

## 特性
* 采用**Reactor**模式来实现，IO模型为**IO multiplexing + non-blocking IO**。
* 基于事件的驱动和回调，支持多种IO多路复用技术，包括select，poll(2)，epoll(4)， kqueue。
* 线程模型为**one loop per thread + threadpool**，每个线程最多有一个事件轮询，每个TCP连接都必须归某一个线程来管理，它所有的IO都在该线程上完成。
* 在单线程的情况下，主线程既处理连接请求，又处理读写事件。在多线程的情况下，主线程只处理连接请求，当连接建立后，立即将该连接交付给其中一个子线程来管理，直至该连接的断开。
* 网络库的线程数目稳定，在正常启动时，会创建好所有的线程，之后不再创建新的线程。这样可以避免因为频繁地创建和销毁线程而带来巨大的性能损耗。
* 主线程根据每个子线程上的连接数来进行动态调度，将新建立的连接分派给当前连接数最少的子线程管理。
* 目前只实现TCP连接，并没有实现UDP连接。
* 采用C++11编写，使用智能指针，RAII手法来管理内存，使用右值语义和移动语义来减少不必要的内存拷贝，使用function + bind来做事件的回调处理，使用lambda表达式来替换传统的函数指针等等。
* 提供完整的网络传输，定时器，RPC远程调用等功能。
* 接口简单易用，示例丰富。

## 局限
* 没有做特别的过载保护，在高压的情况下可能会导致服务不可用。
* 没有做特别的安全保护，需要经过二次开发才能应用于外网环境。
* 只支持Linux，Mac OS X, FreeBSD等类Unix系统, 不支持Windows。

## 使用场景
* 可用于公司内部的大规模分布式系统中，如在Skywalker中用来处理Paxos集群的网络通信。
* 可用于游戏服务器，但如果还需要UDP传输的话，需要自行开发。
* 可用于高频交易系统，类似于sudu库这种高频计算场景。

## 性能
对Voyager做了不少的对比测试，测试代码位于benchmarks目录中。
<br/>
* 吞吐量的测试采用了asio的测试方法,对应代码为benchmarks/client.cc和benchmarks/server.cc，可使用已经写好的shell脚本来启动，链接为http://think-async.com/Asio/LinuxPerformanceImprovements 。
* 事件处理效率的测试采用了libevent的测试方法，对应代码为benchmarks/bench.cc, 链接为http://libev.schmorp.de/bench.html 。

测试结果表明，Voyager在吞吐量和并发量方面表现优异，和asio吞吐量、libevent事件处理效率相当。使用者可以根据测试代码在不同的环境下自行测试。

## 兼容性
Voyager支持Linux，Mac OS X, FreeBSD等类Unix平台，不支持Windows平台，以下是一些曾测试的平台/编译器组合：
* Linux 4.4.0，GCC 5.4.0 
* macOS 10.12，Clang 3.6.0

## 编译安装
(1) Protobuf编译安装(https://github.com/google/protobuf/blob/master/src/README.md) (voyager_rpc库依赖，可选择不安装)
* 进入third_party/protobuf目录 
* 执行 ./autogen.sh
* 执行 ./configure 
* 执行 make && sudo make install

(2) Voyager编译安装
* 进入voyager的根目录
* 执行./build.sh
* 进入./build/release目录
* 执行sudo make install（默认安装目录为/usr/local，可通过修改build.sh文件来修改安装选项)
