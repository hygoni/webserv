#ifndef SRCS_ABNF_HPP_
#define SRCS_ABNF_HPP_
#include "Context.hpp"

/* Copyright 2020 hyeyoo, hyekim */

namespace ABNF {
	bool Alpha(Context &c);
	bool Bit(Context &c);
	bool Char(Context &c);
	bool Cr(Context &c);
	bool Crlf(Context &c, bool is_strict = false);
	bool Ctl(Context &c);
	bool Digit(Context &c);
	bool Dquote(Context &c);
	bool Hexdig(Context &c);
	bool Htap(Context &c);
	bool Lf(Context &c);
	bool Octet(Context &c);
	bool Sp(Context &c);
	bool Vchar(Context &c);
	bool Wsp(Context &c);
	bool Icar(Context &c, char __c);
	bool Scar(Context &c, char __c);
}

#endif
