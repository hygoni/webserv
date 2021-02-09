#ifndef SRCS_UTILS_HPP_
#define SRCS_UTILS_HPP_

#include <string>
#include <limits.h>
#include <time.h>
#include <stdlib.h>
#include "libft.h"

/* 2000-03-01 (mod 400 year, immediately after feb29 */
#define LEAPOCH (946684800LL + 86400*(31+29))
#define DAYS_PER_400Y (365*400 + 97)
#define DAYS_PER_100Y (365*100 + 24)
#define DAYS_PER_4Y   (365*4   + 1)

int           set_tm(long long t, struct tm *tm);
std::string   to_string(int n);

#endif  // SRCS_UTILS_HPP_
