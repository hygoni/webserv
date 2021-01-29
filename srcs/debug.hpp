#ifndef SRCS_DEBUG_HPP_
#define SRCS_DEBUG_HPP_

// #define DEBUG
//#define DEBUG_SLEEP

#ifdef DEBUG
 #include <cstdio>
 #define debug_printf printf
#endif

#ifndef DEBUG
 #define debug_printf(x, ...)
#endif

#define BUFSIZE 1048576

#include <stdexcept>
#include <cstdlib>
#include <execinfo.h>
#include <iostream>

void handler();

#endif  // SRCS_DEBUG_HPP_
