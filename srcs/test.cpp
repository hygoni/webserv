#include <iostream>
#include "Parser.hpp"
#include "abnf.hpp"

// "010-" 3*4DIGIT "-" 4*DIGIT
bool test(std::string str) {
	Context ctx(str);

	AcceptTemplate<char>(ctx, '0', ABNF::Scar).run();
	AcceptTemplate<char>(ctx, '1', ABNF::Scar).run();
	AcceptTemplate<char>(ctx, '0', ABNF::Scar).run();
	AcceptTemplate<char>(ctx, '-', ABNF::Scar).run();
	Repeat(ctx, 3, 4, new Accept(ctx, ABNF::Digit)).run();
	return ctx.state;
}


int main(int, char **argv) {
	std::string str(argv[1]);
	if (test(str))
		std::cout << "true" << std::endl;
	else
		std::cout << "false" << std::endl;
}