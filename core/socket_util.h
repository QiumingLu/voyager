#ifndef MIRANTS_CORE_SOCKET_UTIL_H_
#define MIRANTS_CORE_SOCKET_UTIL_H_

#include <stdint.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include "util/status.h"

namespace mirants {
namespace sockets {
  
int CreateSocket(int domain);
int CreateSocketAndSetNonBlock(int domain);
void CloseFd(int socketfd);
void BindAddress(int socketfd, const struct sockaddr* sa, socklen_t salen);
void Listen(int socketfd, int backlog);
int Accept(int socketfd, struct sockaddr* sa, socklen_t salen);
int Connect(int socketfd, const struct sockaddr* sa, socklen_t salen);

ssize_t Read(int socketfd, void* buf, size_t count);
ssize_t ReadV(int socketfd, const struct iovec* iov, int count);
ssize_t Write(int socketfd, void* buf, size_t count);
ssize_t WriteV(int socketfd, const struct iovec* iov, int count);
void ShutDownWrite(int socketfd);

Status SetBlocking(int socketfd, bool blocking);
Status SetReuseAddr(int socketfd, bool reuse);
Status SetReusePort(int socketfd, bool reuse);
Status SetKeepAlive(int socketfd, bool alive);
Status SetTcpNoDelay(int socketfd, bool notdelay);
Status CheckSocketError(int socketfd);

Status Resolve(char* hostname, char* ipbuf, size_t ipbuf_size);
Status ResolveIP(char* hostname, char* ipbuf, size_t ipbuf_size);

int FormatAddr(const char* ip, uint16_t port, char* buf, size_t buf_size);
int FormatPeer(int socketfd, char* buf, size_t buf_size);
int FormatLocal(int socketfd, char* buf, size_t buf_size);

void SockAddrToIP(const struct sockaddr* sa, char* ipbuf, size_t ipbuf_size);
int SockAddrToIPPort(const struct sockaddr* sa, char* buf, size_t buf_size);
void IPPortToSockAddr(const char* ip, uint16_t port, struct sockaddr_in* sa4);
void IPPortToSockAddr(const char* ip, uint16_t port, struct sockaddr_in6* sa6);

struct sockaddr_storage PeerSockAddr(int socketfd);
struct sockaddr_storage LocalSockAddr(int socketfd);

}  // namespace sockets
}  // namespace mirants 


// #include <netinet/in.h>

// All pointers to socket address structures are often cast to pointers
// to this type before use in various functions and system calls:

// struct sockaddr {
//   unsigned short    sa_family;    // address family, AF_xxx
//   char              sa_data[14];  // 14 bytes of protocol address
// };


// IPv4 AF_INET sockets:

// struct sockaddr_in {
//     short            sin_family;   // e.g. AF_INET, AF_INET6
//     unsigned short   sin_port;     // e.g. htons(3490)
//     struct in_addr   sin_addr;     // see struct in_addr, below
//     char             sin_zero[8];  // zero this if you want to
// };

// struct in_addr {
//     unsigned long s_addr;          // load with inet_pton()
// };


// IPv6 AF_INET6 sockets:

// struct sockaddr_in6 {
//     u_int16_t       sin6_family;   // address family, AF_INET6
//     u_int16_t       sin6_port;     // port number, Network Byte Order
//     u_int32_t       sin6_flowinfo; // IPv6 flow information
//     struct in6_addr sin6_addr;     // IPv6 address
//     u_int32_t       sin6_scope_id; // Scope ID
// };

// struct in6_addr {
//     unsigned char   s6_addr[16];   // load with inet_pton()
// };


// General socket address holding structure, big enough to hold either
// struct sockaddr_in or struct sockaddr_in6 data:

// struct sockaddr_storage {
//     sa_family_t  ss_family;     // address family

//     // all this is padding, implementation specific, ignore it:
//     char      __ss_pad1[_SS_PAD1SIZE];
//     int64_t   __ss_align;
//     char      __ss_pad2[_SS_PAD2SIZE];
// };

// Example:
// IPv4:

// struct sockaddr_in ip4addr;
// int s;

// ip4addr.sin_family = AF_INET;
// ip4addr.sin_port = htons(3490);
// inet_pton(AF_INET, "10.0.0.1", &ip4addr.sin_addr);

// s = socket(PF_INET, SOCK_STREAM, 0);
// bind(s, (struct sockaddr*)&ip4addr, sizeof ip4addr);


// IPv6:

// struct sockaddr_in6 ip6addr;
// int s;

// ip6addr.sin6_family = AF_INET6;
// ip6addr.sin6_port = htons(4950);
// inet_pton(AF_INET6, "2001:db8:8714:3a90::12", &ip6addr.sin6_addr);

// s = socket(PF_INET6, SOCK_STREAM, 0);
// bind(s, (struct sockaddr*)&ip6addr, sizeof ip6addr);


// struct addrinfo {
//   int     ai_flags;
//   int     ai_family;
//   int     ai_socktype;
//   int     ai_protocol;
//   size_t  ai_addrlen;
//   struct  sockaddr* ai_addr;
//   char*   ai_canonname;     /* canonical name */
//   struct  addrinfo* ai_next; /* this struct can form a linked list */
// };


// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netdb.h>

// int getaddrinfo(const char *hostname, // e.g. "www.example.com" or IP
//                 const char *service,  // e.g. "http" or port number
//                 const struct addrinfo *hints,
//                 struct addrinfo **res);

// hostname:一个主机名或者地址串(IPv4的点分十进制串或者IPv6的16进制串)
// service：服务名可以是十进制的端口号，也可以是已定义的服务名称，如ftp、http等
// hints：可以是一个空指针，也可以是一个指向某个addrinfo结构体的指针，调用者在
//        这个结构中填入关于期望返回的信息类型的暗示。举例来说：如果指定的服务
//        既支持TCP也支持UDP，那么调用者可以把hints结构中的ai_socktype成员设置
//        成SOCK_DGRAM使得返回的仅仅是适用于数据报套接口的信息。
// result：本函数通过result指针参数返回一个指向addrinfo结构体链表的指针。
// 返回值：0——成功，非0——出错


// #include <sys/socket.h>
// #include <netdb.h>

// int getnameinfo(const struct sockaddr* sa, socklen_t salen,
//                 char* host, size_t hostlen,
//                 char* serv, size_t servlen,
//                 int flags);

// #include <sys/socket.h>
// #include <netdb.h>

// void freeaddrinfo(struct addrinfo *ai);
// ai参数应指向由getaddrinfo返回的第一个addrinfo结构。这个连表中的所有结构以及
// 它们指向的任何动态存储空间都被释放掉。


// int status;
// struct addrinfo hints;
// struct addrinfo *servinfo;  // will point to the results

// memset(&hints, 0, sizeof hints); // make sure the struct is empty
// hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
// hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
// hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

// #include<netdb.h>
// const char *gai_strerror( int error );
// 该函数以getaddrinfo返回的非0错误值的名字和含义为他的唯一参数，返回一个指向对应的出错信息串的指针。


// if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
//     fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
//     exit(1);
// }

// // servinfo now points to a linked list of 1 or more struct addrinfos

// // ... do everything until you don't need servinfo anymore ....

// freeaddrinfo(servinfo); // free the linked-list


// #define _BSD_SOURCE         /* See feature_test_macros(7) */
// #include <endian.h>

// uint16_t htobe16(uint16_t host_16bits);
// uint16_t htole16(uint16_t host_16bits);
// uint16_t be16toh(uint16_t big_endian_16bits);
// uint16_t le16toh(uint16_t little_endian_16bits);

// uint32_t htobe32(uint32_t host_32bits);
// uint32_t htole32(uint32_t host_32bits);
// uint32_t be32toh(uint32_t big_endian_32bits);
// uint32_t le32toh(uint32_t little_endian_32bits);

// uint64_t htobe64(uint64_t host_64bits);
// uint64_t htole64(uint64_t host_64bits);
// uint64_t be64toh(uint64_t big_endian_64bits);

#endif  // MIRANTS_CORE_SOCKET_UTIL_H_
