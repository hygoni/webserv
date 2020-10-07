#include "Context.hpp"

Context::Context(std::string const & str) {
	this->str = str;
	this->idx = 0;
	this->level = 0;
	this->skip = -1;
	this->state = true;
}

const char& Context::current() const {
	return str.at(idx);
}

void Context::next() {
	this->idx++;
}

