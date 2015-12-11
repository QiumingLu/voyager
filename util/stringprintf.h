#ifndef MIRANTS_UTIL_STRINGPRINTF_H_
#define MIRANTS_UTIL_STRINGPRINTF_H_

#include <string>
#include <stdio.h>
#include <stdarg.h>

namespace mirants {

// Lower-level routine that takes a va_list and appends to a specified
// string. All other routines are just convenience wrappers around it.
extern void StringAppendV(std::string* dst, const char* format, va_list ap);

// Return a C++ string.
extern std::string StringPrintf(const char* format, ...);

// Store result into a supplied string and return it. 
// The previous dst will be clear.
extern const std::string& SStringPrintf(std::string* dst, 
                                        const char* format, ...);

// Append result into a supplied string.
extern void StringAppendF(std::string* dst, const char* format, ...);

}  // namespace mirants

#endif  // MIRANTS_UTIL_STRINGPRINTF_H_
