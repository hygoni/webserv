#ifndef CONTEXT_HPP_
#define CONTEXT_HPP_

#include <string>
#include <stack>

struct Context {
	int				idx;
	std::stack<int>	save;
	int				skip;
	int				level;
	bool			state;
	std::string		str;
					Context(std::string const & str);
	char			current() const;
	void			next();
};

#endif  // CONTEXT_HPP_
