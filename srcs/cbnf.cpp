/* Copyright 2020 hyeyoo, hyekim */

#include "Context.hpp"
#include "abnf.hpp"
#include <string>
#include "../includes/libft.h"

/* CBNF : Customized BNF rules */
namespace CBNF {

	bool	String(Context& c, std::string str) {
		for (unsigned long i = 0; i < str.size(); i++) {
			if (!ABNF::Scar(c, str[i]))
				return false;
		}
		return true;
	}
}  // namespace CBNF
