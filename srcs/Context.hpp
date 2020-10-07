#ifndef SRCS_CONTEXT_HPP_
#define SRCS_CONTEXT_HPP_

/* Copyright 2020 hyeyoo, hyekim */

#include <string>
#include <stack>

struct Context {
    int                idx;
    std::stack<int>    save;
    int                skip;
    int                level;
    bool               state;
    std::string        str;
    explicit           Context(std::string const & str);
    const char&        current() const;
    void               next();
};

#endif  // SRCS_CONTEXT_HPP_
