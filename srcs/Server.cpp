/* Copyright 2020 hyeyoo */

#include <utility>
#include "ConfigParser.hpp"
#include "Server.hpp"
#include "cbnf.hpp"
#include "abnf.hpp"
#include "Parser.hpp"

/*
 * TODO: 각각 Attribute 별로 파싱도 해야함
 * key, value에 대한 rule 정하기
 */

Server::Server(std::string const& str) : _ctx(str) {}

/*
 * TODO: rule 재정의하기
 * server *WSP { LF
 * 1*(location | attr)
 * } *LF
 */
void Server::parseServer() {
    AcceptTemplate<std::string>(this->_ctx, "server", CBNF::String).run();
    Repeat(this->_ctx, 0, 0, new Accept(this->_ctx, ABNF::Wsp)).run();
    AcceptTemplate<char>(this->_ctx, '{', ABNF::Scar).run();
    Repeat(this->_ctx, 0, 0, new Accept(this->_ctx, ABNF::Wsp)).run();
    Repeat(this->_ctx, 1, 0, new Accept(this->_ctx, ABNF::Lf)).run();
    Repeat(this->_ctx, 1, 0, (new Group(this->_ctx))->add(new Accept(this->_ctx, ConfigParser::parseAttribute))
            ->add(new Alternate(this->_ctx))
            ->add(new Accept(this->_ctx, ConfigParser::parseLocation)));
    AcceptTemplate<char>(this->_ctx, '}', ABNF::Scar).run();
    Repeat(this->_ctx, 0, 0, new Accept(this->_ctx, ABNF::Wsp)).run();
    Repeat(this->_ctx, 0, 0, new Accept(this->_ctx, ABNF::Lf)).run();
    this->_ctx.end();
}

void Server::setAttribute
(std::string const& key, std::string const& value) {
    this->_attrs.insert(std::make_pair(key, value));
}

std::map<std::string, std::string>::const_iterator Server::getAttribute
(std::string const& key) const {
    return this->_attrs.find(key);
}

void Server::addLocation(Location location) {
    this->_locations.push_back(location);
}

std::vector<Location>::const_iterator Server::getLocations() const {
    return this->_locations.begin();
}
