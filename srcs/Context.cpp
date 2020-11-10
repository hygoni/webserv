/* Copyright 2020 hyeyoo, hyekim */

#include <climits>
#include "Context.hpp"

Context::Context(std::string const & str) {
    this->str = str;
    this->idx = 0;
    this->level = 0;
    this->skip = INT_MAX;
    this->state = true;
    this->level++;
    this->save.push(this->idx);
}

void    Context::end() {
    this->level--;
    if ((unsigned long)(this->idx) != this->str.size())
        this->state = false;
    this->save.pop();
}

const char& Context::current() const {
    return str.at(idx);
}

void Context::next() {
    this->idx++;
}

