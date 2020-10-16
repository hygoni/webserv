/* Copyright 2020 hyeyoo, hyekim */

#include "Context.hpp"
#include "abnf.hpp"
#include "construct.hpp"
#include <string>
#include "../includes/libft.h"

/* CBNF : Customized BNF rules */
namespace CBNF {

	bool	String(Context& c, std::string const& str) {
		for (unsigned long i = 0; i < str.size(); i++) {
			if (!ABNF::Scar(c, str[i]))
				return false;
		}
		return true;
	}

  // ALPHA / DIGIT / "-" / "." / "_" / "~"
  bool  Unreserved(Context& c) {
    if (ft_isalnum(c.current()) ||
        std::string("-._~").find(c.current()) != std::string::npos) {
      c.next();
      return true;
    }
    return false;
  }

  // sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
  //                  / "*" / "+" / "," / ";" / "="
  bool  SubDelims(Context& c) {
    if (std::string("!$&'()*+,;=").find(c.current()) != std::string::npos) {
      c.next();
      return true;
    }
    return false;
  }

  bool  Tchar(Context& c) {
    // tchar          = "!" / "#" / "$" / "%" / "&" / "'" / "*"
    //               / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
    //               / DIGIT / ALPHA
    if (ft_isalnum(c.current()) ||
        std::string("!#$%&'*+-.^_`|~").find(c.current()) != std::string::npos) {
      c.next();
      return true;
    }
    return false;
  }

  // pct-encoded   = "%" HEXDIG HEXDIG
  bool  PctEncoded(Context& c) {
    c.save.push(c.idx);
    if (ABNF::Scar(c, '%') && ABNF::Hexdig(c) && ABNF::Hexdig(c)) {
      c.save.pop();
      return true;
    }
    c.idx = c.save.top();
    c.save.pop();
    return false;
  }

  bool  Pchar(Context& c) {
    // pchar         = unreserved / ":" / "@" / sub-delims / pct-encoded
    if (ft_isalnum(c.current()) ||
        std::string("-._~:@!$&'()*+,;=").find(c.current()) != std::string::npos) {
      c.next();
      return true;
    }
    c.save.push(c.idx);
    if (ABNF::Scar(c, '%') && ABNF::Hexdig(c) && ABNF::Hexdig(c)) {
      c.save.pop();
      return true;
    }
    c.idx = c.save.top();
    c.save.pop();
    return false;
  }

  bool  Token(Context& c) {
    repeat(c, 1, 0, Tchar);
    return c.state;
  }
}  // namespace CBNF
