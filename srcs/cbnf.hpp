#ifndef SRCS_CBNF_HPP_
#define SRCS_CBNF_HPP_
#include "Context.hpp"
#include <string>

/* Copyright 2020 hyeyoo, hyekim */

namespace CBNF {
	bool String(Context &c, std::string const& str);
  bool  Tchar(Context& c);
  bool  Pchar(Context& c);
  bool  Token(Context& c);
}

#endif
