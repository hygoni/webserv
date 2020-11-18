#include "Request.hpp"
#include "libft.h"
// HTTP-message   = start-line
//                           *( header-field CRLF )
//                           CRLF
//                           [ message-body ]
// start-line = method SP request-target SP HTTP-version CRLF
// header-field = field-name ":" OWS field-value OWS

Request::~Request() {
}

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
  field_value = std::string("");
  if (sep_i != header_field.length() - 1) {
    field_value = header_field.substr(sep_i + 1);
    if (field_value.at(0) == ' ')
      field_value = field_value.substr(1);
  }
  checkOverlapHeader(field_name, field_value);
  return (std::make_pair(field_name, field_value));
}

void    Request::initHeaders(std::string raw) {
  size_t      start = 0;
  std::string header_field;

  for (size_t i = 0; raw.length() && i < raw.length() - 1; i++) {
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

void  Request::checkOverlapHeader(const std::string & name, const std::string & value) const {
  std::map<std::string, std::string>::const_iterator it;
  if (name == "Host" && _headers.find("Host") != _headers.end())
    throw HttpException(400);

  if (_headers.find("Transfer-Encoding") == _headers.end())
    return ;
  if (name == "Content-Length" && (it = _headers.find("Content-Length")) != _headers.end()
      && it->second != value)
    throw HttpException(400);
}

void  Request::checkHeaders() {
  std::map<std::string, std::string>::iterator it;
  size_t  i;

  if (_headers.find("Host") == _headers.end())
    throw HttpException(400);
  if ((it = _headers.find("Transfer-Encoding")) != _headers.end()) {
    if (it->second != "chunked")
      throw HttpException(400); // TODO: abnf OWS ',' required
    _headers.erase("Content-Length");
    _chunked = true;
  } else if ((it = _headers.find("Content-Length")) != _headers.end()) {
    for (i = 0; i < it->second.length() && ft_isdigit(it->second.at(i)); i++) {
      _content_length = _content_length * 10 + it->second.at(i) - '0';
    }
    if (i == 0 || i != it->second.length())
      throw HttpException(400);
  }
}

Request::Request(std::string http_message) {
  _is_closed = false;
  _chunked = false;
  _content_length = 0;
  size_t  start_line_crlf = http_message.find("\r\n");
  if (start_line_crlf == std::string::npos)
    throw HttpException(400);
  initStartLine(http_message.substr(0, start_line_crlf));
  size_t  header_end_crlf = http_message.find("\r\n\r\n", start_line_crlf + 2);
  initHeaders(http_message.substr(start_line_crlf + 2, header_end_crlf + 2));
  checkHeaders();
  if (_chunked == false && _content_length == 0)
    _is_closed = true;
  this->addBody(http_message.substr(header_end_crlf + 2));
  std::cout << _is_closed << " CL: " << _content_length << std::endl;
}

void  Request::addBody(const std::string & str) {
  std::string   extra;
  std::map<std::string, std::string>::iterator it;

  if (_is_closed)
    return ;
  if (_chunked) {
    extra = parseChunk(str);
  } else {
    extra = str;
  }
  if (extra.length() + _body.length() > MAX_BODY_SIZE)
    throw HttpException(413);
  _body += extra;
  if (_body.length() >= _content_length) {
    _body = _body.substr(0, _content_length);
    _is_closed = true;
  }
}

void  Request::checkClosed() {

}
// chunked-body   = *chunk
//                                last-chunk
//                               trailer-part
//                               CRLF
// chunk           = chunk-size [ chunk-ext ] CRLF
//                         chunk-data CRLF
// chunk-size    = 1*HEXDIG
// last-chunk    = 1*("0") [ chunk-ext ] CRLF
// chunk-data     = 1*OCTET
static bool  isHex(char c) {
  if (('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'))
    return true;
  return false;
}

std::string  Request::parseChunk(const std::string & str) {
  size_t  chunk_size = 0;
  size_t  i;

  for (i = 0; i < str.length() && isHex(str.at(i)); i++) {
    chunk_size *= 16;
    if (ft_isdigit(str.at(i)))
      chunk_size += str.at(i) - '0';
    else
      chunk_size += ft_tolower(str.at(i)) - 'a' + 16;
  }
  if (i == 0 || str.length() - i != chunk_size + 4
      || (str.at(i) != '\r' && str.at(i + 1) != '\n')
      || (str.at(str.length() - 2) != '\r' && str.back() != '\n')) {
    throw HttpException(400);
  }
  if (chunk_size == 0) {
    _is_closed = true;
    return std::string();
  }
  return str.substr(i + 2, chunk_size);
}

bool  Request::isClosed() const {
  return _is_closed;
}

std::string Request::getMethod() const {
  return _method;
}

std::string Request::getTarget() const {
  return _target;
}

std::string Request::getVersion() const {
  return _version;
}

std::string Request::getBody() const {
  return _body;
}

std::map<std::string, std::string> Request::getHeaders() const {
  return _headers;
}


void  Request::debugOstream(std::ostream& os) const {
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
