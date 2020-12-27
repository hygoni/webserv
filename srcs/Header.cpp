#include "Header.hpp"
#include "Message.hpp"
#include "libft.h"

Header::Header() {
}

Header::Header(std::string method, std::string target, std::string version) {
  _method = method;
  _target = target;
  _version = version;
}

Header::Header(int status) {
  _status = status;
  _message = Message::getInstance()->getMessage(status);
}

std::string&        Header::operator[](const std::string& key) {
  return _items[key];
}

const std::string  Header::operator[](const std::string& key) const {
  if (!isExist(key))
    return std::string();
  return _items.find(key)->second;
}

size_t              Header::erase(const std::string& key) {
  return _items.erase(key);
}

bool                Header::isExist(const std::string& key) const {
  std::map<std::string, std::string>::const_iterator it;

  return _items.find(key) != _items.end();
}

std::string         Header::toString() const {
  std::map<std::string, std::string>::const_iterator  it;
  std::string result = "HTTP/1.1 ";

  result += _status / 100 + '0';
  result += _status / 10 % 10 + '0';
  result += _status % 100 + '0';
  result += ' ' + _message + "\r\n";

  for (it = _items.begin(); it != _items.end(); it = std::next(it)) {
    result += it->first + ": " + it->second + "\r\n";
  }
  result += "\r\n";
  return result;
}

const std::string&  Header::getMethod() const {
  return _method;
}

const std::string&  Header::getTarget() const {
  return _target;
}

const std::string&  Header::getVersion() const {
  return _version;
}

int                 Header::getStatus() const {
  return _status;
}

const std::string&  Header::getMessage() const {
  return _message;
}

int                 Header::getContentLength() {
  if (_items.find("Content-Length") == _items.end()) {
    return -1;
  } else {
    return ft_atoi(_items["Content-Length"].c_str());
  }
}