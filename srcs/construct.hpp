#ifndef SRCS_CONSTRUCT_HPP_
#define SRCS_CONSTRUCT_HPP_
#include "Context.hpp"
/* Copyright 2020 hyeyoo, hyekim */

void begin(Context& ctx);
void end(Context& ctx);
void group(Context& ctx);
void ungroup(Context& ctx);
void repeat(Context& ctx, int min, int max, bool (*f)(Context&));
void repeat(Context& ctx, char ch, int min, int max, bool (*f)(Context&, char));
void repeat(Context& ctx, bool b, int min, int max, bool (*f)(Context&, bool));
void option(Context& ctx, void (*f)(Context&));
void accept(Context& ctx, bool (*f)(Context&));
void accept(Context& ctx, bool b, bool (*f)(Context&, bool));
void accept(Context& ctx, char ch, bool (*f)(Context&, char));
void alternate(Context& ctx);

#endif