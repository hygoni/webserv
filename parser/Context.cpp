#include "Context.hpp"

Context::Context(std::string const & str) {
	this->str = str;
	this->idx = 0;
	this->level = 0;
	this->skip = -1;
	this->state = true;
}

char Context::current() const {
	return str[idx];
}

void Context::next() {
	this->idx++;
}

