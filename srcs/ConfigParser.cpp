/* Copyright 2020 hyeyoo */

#include "ConfigParser.hpp"
#include "Parser.hpp"
#include "abnf.hpp"
#include "cbnf.hpp"

/*
 * KEY = 1*VCHAR
 */
bool ConfigParser::parseKey(Context &ctx) {
    Repeat(ctx, 1, 0, new Accept(ctx, ABNF::Vchar));
    return ctx.state;
}

/*
 * VALUE = 1*VCHAR
 */
bool ConfigParser::parseValue(Context& ctx) {
    Repeat(ctx, 1, 0, new Accept(ctx, ABNF::Vchar));
    return ctx.state;
}

/*
 * ATTRIBUTE = TOKEN 1*WSP VALUE LF
 */
bool ConfigParser::parseAttribute(Context& ctx) {
    Accept(ctx, parseKey).run();
    Repeat(ctx, 1, 0, new Accept(ctx, ABNF::Wsp)).run();
    Accept(ctx, parseValue).run();
    Accept(ctx, ABNF::Lf).run();
    return ctx.state;
}

bool ConfigParser::parseLocation(Context& ctx) {
    AcceptTemplate<std::string>(ctx, "location", CBNF::String).run();
    Repeat(ctx, 0, 0, new Accept(ctx, ABNF::Wsp)).run();
    AcceptTemplate<char>(ctx, '{', ABNF::Scar).run();
    Repeat(ctx, 0, 0, new Accept(ctx, ABNF::Wsp)).run();
    Repeat(ctx, 1, 0, new Accept(ctx, ABNF::Lf)).run();
    Repeat(ctx, 1, 0, (new Accept(ctx, ConfigParser::parseAttribute))).run();
    AcceptTemplate<char>(ctx, '}', ABNF::Scar).run();
    Repeat(ctx, 0, 0, new Accept(ctx, ABNF::Wsp)).run();
    Repeat(ctx, 0, 0, new Accept(ctx, ABNF::Lf)).run();
    ctx.end();
    return ctx.state;
}
