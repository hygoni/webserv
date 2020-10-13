/* Copyright 2020 hyeyoo, hyekim */

#include "Context.hpp"
#include "abnf.hpp"
#include "../includes/libft.h"

/* CBNF : Customized BNF rules */
namespace CBNF {

	bool	String(Context& c, char *str) {
		while (*str != '\0') {
			if (!ABNF::Scar(c, *str))
				return false;
			str++;
		}
		return true;
	}
}  // namespace CBNF
