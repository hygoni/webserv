#include "Request.hpp"
#include "libft.h"
// HTTP-message   = start-line
//                           *( header-field CRLF )
//                           CRLF
//                           [ message-body ]
// start-line = method SP request-target SP HTTP-version CRLF
// header-field = field-name ":" OWS field-value OWS

void    Request::initMethod(std::string method) {
  this->_method = method;
}

void    Request::initTarget(std::string target) {
  if (target.length() > 8000)
    throw HttpException(414);
  this->_target = target;
}

void    Request::initVersion(std::string version) {
  if (version != "HTTP/1.1")
    throw HttpException(400);
  this->_version = version;
}

void    Request::initStartLine(std::string raw) {
  size_t  start = 0;
  size_t  space_count = 0;

  for (size_t i = 0; raw.length() > 0 && i < raw.length() - 1; i++) {
    if (raw.at(i) == ' ') {
      space_count++;
      if (space_count == 1) {
        this->initMethod(raw.substr(start, i - start));
      } else if (space_count == 2) {
        this->initTarget(raw.substr(start, i - start));
        this->initVersion(raw.substr(i + 1));
        return ;
      } 
      start = i + 1;
    }
  }
}

std::pair<std::string, std::string>    Request::makeHeader(std::string header_field) {
  size_t  sep_i = header_field.find(":");
  std::string field_name, field_value;
  if (sep_i == std::string::npos)
    throw HttpException(400);
  field_name = header_field.substr(0, sep_i);
  if (field_name.length() < 1 || field_name.front() == ' ' || field_name.back() == ' ')
    throw HttpException(400);
  if (field_name == "Host" && _headers.find("Host") != _headers.end())
    throw HttpException(400);
  field_value = std::string("");
  if (sep_i != header_field.length() - 1) {
    field_value = header_field.substr(sep_i + 1);
    if (field_value.at(0) == ' ')
      field_value = field_value.substr(1);
  }
  return (std::make_pair(field_name, field_value));
}

void    Request::initHeaders(std::string raw) {
  size_t      start = 0;
  std::string header_field;

  for (size_t i = 0; i < raw.length() - 1; i++) {
    if (raw.at(i) == '\r' && raw.at(i + 1) == '\n') {
      if (start == i)
        return ;
      header_field = raw.substr(start, i - start);
      this->_headers.insert(makeHeader(header_field));
      start = i + 2;
    }
  }
  throw HttpException(400);
}

Request::Request(std::string http_message) {
  size_t start_line_crlf = http_message.find("\r\n");
  if (start_line_crlf == std::string::npos)
    throw HttpException(400);
  initStartLine(http_message.substr(0, start_line_crlf));
  initHeaders(http_message.substr(start_line_crlf + 2));
  if (_headers.find("Host") == _headers.end())
    throw HttpException(400);
}

void    Request::debugOstream(std::ostream& os) const {
  os << "-----Debug Request-----\n" \
  << "Method: \"" << _method << "\"\n" \
  << "Target: \"" << _target << "\"\n" \
  << "Version: \"" << _version << "\"\n" \
  << "---------Header--------\n";
  std::map<std::string, std::string>::const_iterator it;
  for (it = _headers.begin(); it != _headers.end(); it = std::next(it))
    os << it->first << ": \"" << it->second << "\"\n"; 
  os << "-----------------------\n";
}

std::ostream& operator<<(std::ostream& os, const Request& request) {
  request.debugOstream(os);
  return os;
}
