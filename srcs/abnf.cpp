/* Copyright 2020 hyeyoo, hyekim */

#include "Context.hpp"
#include "../includes/libft.h"

namespace ABNF {
    bool    Alpha(Context& c) {
        if (ft_isalpha(c.current())) {
            c.next();
            return true;
        } else {
            return false;
        }
    }

    bool    Bit(Context& c) {
        if (c.current() == '0' || c.current() == '1') {
            c.next();
            return true;
        } else {
            return false;
        }
    }

    bool    Char(Context& c) {
        if (0x1 <= c.current() && c.current() <= 0x7F) {
            c.next();
            return true;
        } else {
            return false;
        }
    }

    bool    Cr(Context& c) {
        if (c.current() == 0x0D) {
            c.next();
            return true;
        } else {
            return false;
        }
    }

    bool    Crlf(Context& c, bool is_strict = false) {
        c.save.push(c.idx);
        if (is_strict) {
            if (c.current() == 0x0D) {
                c.next();
                if (c.current() == 0x0A) {
                    c.next();
                    c.save.pop();
                    return true;
                }
            }
        } else {
            if (c.current() == 0x0D) {
                c.next();
            }
            if (c.current() == 0x0A) {
                c.next();
                c.save.pop();
                return true;
            }
        }
        c.idx = c.save.top(); c.save.pop();
        return false;
    }

    bool    Ctl(Context& c) {
        if ((0x00 <= c.current() && c.current() <= 0x1F)
                || c.current() == 0x7F) {
            c.next();
            return true;
        }
        return false;
    }

    bool    Digit(Context& c) {
        if (ft_isdigit(c.current())) {
            c.next();
            return true;
        }
        return false;
    }

    bool    Dquote(Context& c) {
        if (c.current() == 0x22) {
            c.next();
            return true;
        }
        return false;
    }

    bool    Hexdig(Context& c) {
        if (ft_isdigit(c.current())
                || ((c.current() >= 'A') && (c.current() <= 'F'))
                || ((c.current() >= 'a') && (c.current() <= 'f'))) {
            c.next();
            return true;
        }
        return false;
    }

    bool    Htap(Context& c) {
        if (c.current() == 0x09) {
            c.next();
            return true;
        }
        return false;
    }

    bool    Lf(Context& c) {
        if (c.current() == 0x0A) {
            c.next();
            return true;
        }
        return false;
    }

    bool    Octet(Context& c) {
        if (c.current()) {
            c.next();
            return true;
        }
        return false;
    }

    bool    Sp(Context& c) {
        if (c.current() == ' ') {
            c.next();
            return true;
        }
        return false;
    }

    bool    Vchar(Context& c) {
         if (c.current() >= '!' && c.current() <= '~') {
            c.next();
            return true;
        }
        return false;
    }

    bool    Wsp(Context& c) {
        if (c.current() == ' ' || c.current() == 0x9) {
            c.next();
            return true;
        }
        return false;
    }

    bool    Icar(Context& c, char __c) {
        if (ft_tolower(c.current()) == ft_tolower(__c)) {
            c.next();
            return true;
        }
        return false;
    }

    bool    Scar(Context& c, char __c) {
        if (c.current() == __c) {
            c.next();
            return true;
        }
        return false;
    }
}  // namespace ABNF
