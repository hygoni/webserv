#ifndef SRCS_DEBUG_HPP_
#define SRCS_DEBUG_HPP_

#define DEBUG
//#define DEBUG_SLEEP

#ifdef DEBUG
 #include <cstdio>
 #define log printf
#endif

#ifndef DEBUG
 #define log(x, ...)
#endif

#define BUFSIZE 1048576

#endif  // SRCS_DEBUG_HPP_
