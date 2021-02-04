#include "Request.hpp"
#include "libft.h"
// HTTP-message   = start-line
//                           *( header-field CRLF )
//                           CRLF
//                           [ message-body ]
// start-line = method SP request-target SP HTTP-version CRLF
// header-field = field-name ":" OWS field-value OWS

Request::~Request() {
  if (_header != NULL)
    delete _header;
  if (_body != NULL)
    delete _body;
}

void    Request::initStartLine(std::string raw) {
  size_t  start = 0;
  size_t  space_count = 0;
  std::string method;
  std::string target;
  std::string version;

  for (size_t i = 0; raw.length() > 0 && i < raw.length() - 1; i++) {
    if (raw.at(i) == ' ') {
      space_count++;
      if (space_count == 1) {
        method = raw.substr(start, i - start);
      } else if (space_count == 2) {
        target = raw.substr(start, i - start);
        version = raw.substr(i + 1);
        if (target.length() > 8000)
          throw HttpException(414);
        if (version != "HTTP/1.1")
          throw HttpException(400);
        _header = new Header(method, target, version);
        return ;
      }
      start = i + 1;
    }
  }
  throw HttpException(400);
}

void    Request::addHeader(std::string header_field) {
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
  (*_header)[field_name] = field_value;
  return ;
}

void    Request::initHeaders(std::string raw) {
  size_t      start = 0;
  std::string header_field;

  for (size_t i = 0; raw.length() && i < raw.length() - 1; i++) {
    if (raw.at(i) == '\r' && raw.at(i + 1) == '\n') {
      if (start == i)
        return ;
      header_field = raw.substr(start, i - start);
      addHeader(header_field);
      start = i + 2;
    }
  }
  throw HttpException(400);
}

void  Request::setBody() {
  if (_chunked) {
    _body = new ChunkedBody();
  } else {
    _body = new Body(_content_length);
  }
}

void  Request::checkOverlapHeader(const std::string & name, const std::string & value) const {
  std::map<std::string, std::string>::const_iterator it;
  if (name == "Host" && _header->isExist("Host"))
    throw HttpException(400);

  if (_header->isExist("Transfer-Encoding"))
    return ;
  if (name == "Content-Length" && _header->isExist("Content-Length")
      && (*_header)["Content-Length"] != value)
    throw HttpException(400);
}

void  Request::checkHeaders() {
  size_t  i;
  size_t  past;

  if (!_header->isExist("Host"))
    throw HttpException(400);
  if (_header->isExist("Transfer-Encoding")) {
    if ((*_header)["Transfer-Encoding"] != "chunked")
      throw HttpException(400); // TODO: abnf OWS ',' required
    _header->erase("Content-Length");
    _chunked = true;
  } else if (_header->isExist("Content-Length")) {
    for (i = 0; i < (*_header)["Content-Length"].length() && ft_isdigit((*_header)["Content-Length"].at(i)); i++) {
      past = _content_length;
      _content_length = _content_length * 10 + (*_header)["Content-Length"].at(i) - '0';
      if (past > _content_length) {
        throw HttpException(400);
      }
    }
    if (i == 0 || i != (*_header)["Content-Length"].length())
      throw HttpException(400);
  }
}

Request::Request(std::string const& http_message) {
  _is_closed = false;
  _chunked = false;
  _body = NULL;
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
}

std::string  Request::parseChunk(const std::string & str) {
  size_t  chunk_size = 0;
  size_t  i;

  for (i = 0; i < str.length(); i++) {
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

bool Request::hasBody() const {
  if (_chunked)
    return true;
  else if (_content_length > 0)
    return true;
  return false;
}

std::string Request::getMethod() const {
  return _header->getMethod();
}

std::string Request::getTarget() const {
  return _header->getTarget();
}

std::string Request::getVersion() const {
  return _header->getVersion();
}

std::string Request::getUserName() const {
  return _user_name;
}

size_t Request::getContentLength() const {
  return _content_length;
}

bool Request::isChunked() const {
  return _chunked;
}

bool Request::auth(std::string const& user_str) {
  std::string token;
  size_t      idx;

  idx = (*_header)["Authorization"].find("Basic ");
  if (idx == std::string::npos && idx != 0)
    return false;
  token = (*_header)["Authorization"].substr(idx + 6);
  if (user_str != Base64::Decode(token))
    return false;
  idx = user_str.find(':');
  _user_name = user_str.substr(idx);
  return true;
}

Body        *Request::getBody() {
  return _body;
}

Header      *Request::getHeader() {
  return _header;
}

void  Request::addBody(std::string const &s) {
  _body->addBody(s);
}

bool  Request::isBodyFinished() const {
  return _body->isFinished();
}

std::string Request::getBodyRemain() const {
  return _body->getRemain();
}
