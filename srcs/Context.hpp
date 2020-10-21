#ifndef SRCS_CONTEXT_HPP_
#define SRCS_CONTEXT_HPP_

/* Copyright 2020 hyeyoo, hyekim */

#include <string>
#include <stack>
#include <climits>

class Context {
public:
    size_t             idx;
    std::stack<int>    save;
    int                skip;
    int                level;
    bool               state;
    std::string        str;

    Context(std::string const & str);
    const char&        current() const;
    void               next();
	void				end();
};

#endif  // SRCS_CONTEXT_HPP_
