#include "Body.hpp"
#include "debug.hpp"
#include <algorithm>

Body::Body(int content_length) {
  _content_length = content_length;
}

Body::~Body() {

}

void Body::addBody(std::string const& s) {
  _body.append(s);
}

bool Body::isFinished() const {
  return ((int)_body.length() >= _content_length);
}

std::string Body::getRemain() const {
  return _body.substr(_content_length);
}

std::string const& Body::toString() const {
  return _body;
}