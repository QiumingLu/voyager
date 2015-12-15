#ifndef MIRANTS_UTIL_TIME_H_
#define MIRANTS_UTIL_TIME_H_

namespace mirants {
namespace timedetail {
  
static const int64_t kSecondsPerMinute = 60;
static const int64_t kSecondsPerHour = 3600;
static const int64_t kSecondsPerDay = kSecondsPerHour * 24;
static const int64_t kMilliSecondsPerSecond = 1000;
static const int64_t kMicroSecondsPerSecond = 1000 * 1000;
static const int64_t kNonasSecondsPerSecond = 1000 * 1000 * 1000;

}  // namespace timedetail
}  // namespace mirants

// // 时间处理函数：

// // time函数是最常用的函数，查询当前时间。当前时间为从UTC时间的1970年0点0分0秒经过的秒数。
// // 精度太低，不适合用于做定时器
// #include <time.h>         
// time_t time(time_t *t);


// struct timeval {    
//   time_t      tv_sec;     /* seconds */    
//   suseconds_t tv_usec;    /* microseconds */ 
// };

// // 该函数也是获得当前时间和当前时区
// // 不是系统调用，而是在用户态实现，没有上下文切换和陷入内核的开销，分辨率是 1 微秒。
// #include <sys/time.h>
// #include <time.h>  
// int gettimeofday(struct timeval *tv, struct timezone *tz)

// // 时区处理函数
// // tz_minuteswest获得当前系统所在时区和UTC的时间差，tz_minuteswest以分钟计算。比
// // 如北京GMT+8区，tz_minuteswest为-480。

// // tz_dsttime的定义为日光节约时间（DST，也就是夏令时。）Linux不支持这个字段。

// struct timezone {     
//   int tz_minuteswest;     /* minutes west of Greenwich */     
//   int tz_dsttime;         /* type of DST correction */ 
// };

// // 该函数将UTC时间（从1970年1月1日0时经历的秒数），转换成当前时区时间。
// // localtime返回一个tm的指针，该指针不能保存，因为他指向的内容可能很快会被改变。
// // 该函数多线程不安全，这个函数是标准C定义的。
// // localtime_r是Linux扩展的函数，该函数调用者需要传入一个tm指针。这个函数是线程安全的。
// #include <time.h>  
// struct tm *localtime(const time_t *timep);  
// struct tm *localtime_r(const time_t *timep, struct tm *result);

// struct tm {      
//   int tm_sec;         /* seconds */      
//   int tm_min;         /* minutes */      
//   int tm_hour;        /* hours */      
//   int tm_mday;        /* day of the month */      
//   int tm_mon;         /* month */      
//   int tm_year;        /* year */      
//   int tm_wday;        /* day of the week */      
//   int tm_yday;        /* day in the year */      
//   int tm_isdst;       /* daylight saving time */ 
// };

// // 把时间（time_t）转换成UTC时间tm。一定需要注意这个tm时间是UTC时间，不是当前时区的时间。
// #include <time.h>  
// struct tm *gmtime ( const time_t * timer );  // 线程不安全
// struct tm *gmtime_r(const time_t *timep, struct tm *result);

#endif  // MIRANTS_UTIL_TIME_H_
