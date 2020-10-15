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
void repeat(Context& ctx, std::string str, int min, int max, bool (*f)(Context&, std::string));
void option(Context& ctx, bool (*f)(Context&));
void option(Context& ctx, char ch, bool (*f)(Context&, char));
void option(Context& ctx, bool b, bool (*f)(Context&, bool));
void option(Context& ctx, std::string str, bool (*f)(Context&, std::string));
void accept(Context& ctx, bool (*f)(Context&));
void accept(Context& ctx, bool b, bool (*f)(Context&, bool));
void accept(Context& ctx, char ch, bool (*f)(Context&, char));
void accept(Context& ctx, std::string str, bool (*f)(Context&, std::string));
void alternate(Context& ctx);

#endif
