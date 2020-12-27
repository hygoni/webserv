#ifndef SRCS_DEBUG_HPP_
#define SRCS_DEBUG_HPP_

#define DEBUG

#ifdef DEBUG
 #include <cstdio>
 #define log printf
#endif

#ifndef DEBUG
 #define log(x, ...)
#endif

#endif  // SRCS_DEBUG_HPP_
