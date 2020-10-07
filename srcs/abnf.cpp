#include "Context.hpp"
#include "../includes/libft.h"

namespace ABNF {
	bool	Hexdig(Context& c) {
		if (((c.current()>='0') && (c.current() <='9')) || ((c.current()>='A') && (c.current() <='F')) || ((c.current()>='a') && (c.current() <='f'))) {
			c.next();
            return true;
        }
		return false;
	}

	bool	Htap(Context& c) {
		if (c.current() == 0x09) {
			c.next();
			return true;
		}
		return false;
	}

	bool	Lf(Context& c) {
		if (c.current() == 0x0A) {
			c.next();
			return true;
		}
		return false;
	}

	bool	Octet(Context& c) {
		if (c.current()) {
			c.next();
			return true;
		}
		return false;
	}
	
	bool	Sp(Context& c) {
		if (c.current() == ' ') {
			c.next();
			return true;
		}
		return false;
	}

	bool	Vchar(Context& c) {
		 if (c.current() >= '!' && c.current() <= '~') {
			c.next();
            return true;
        }
		return false;
	}

	bool	Wsp(Context& c) {
		if (c.current() == ' ' || c.current() == 0x9) {
			c.next();
			return true;
        }
		return false;
	}

	bool	Icar(Context& c, char __c) {
		if (c.current() == __c || ((c.current() >= 'a' && c.current() <= 'z') && ((c.current() + 'A' - 'a') == __c))) {
			c.next();
			return true;
        }
		return false;
	}

	bool	Scar(Context& c, char __c) {
		if (c.current() == __c) {
			c.next();
			return true;
		}
		return false;
	}
}
