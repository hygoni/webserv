/* Copyright 2020 hyeyoo */

#include <stdlib.h>
#include <exception>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include "libft.h"
#include "Fd.hpp"
#include "Header.hpp"
#include "CgiBody.hpp"
#include "debug.hpp"

CgiBody::CgiBody(Header **header) : Body() {
  _is_header_closed = false;
  _is_body_closed = false;
  _is_header_sent = false;
  _header = header;
  _pos = 0;
}

CgiBody::CgiBody(const std::string& s, Header **header) : Body(s) {
  _is_header_closed = false;
  _is_body_closed = false;
  _is_header_sent = false;
  _header = header;
  _pos = 0;
}

CgiBody::~CgiBody() {
}

void CgiBody::parse
(int& status, std::map<std::string, std::string>& parse_map) {
  size_t curr;
  size_t prev;
  std::vector<std::string> lines;

  /* if no status in output, we can assume it 200 */
  status = 200;
  /* iterate each line */
  prev = 0;
  curr = _raw_header.find('\n');
  while (curr != std::string::npos) {
    std::string line = _raw_header.substr(prev, curr - prev);
    lines.push_back(line);
    prev = curr + 1;
    curr = _raw_header.find('\n', prev);
  }
  std::string line = _raw_header.substr(prev);
  lines.push_back(line);

  for (size_t i = 0; i < lines.size(); i++) {
    std::string const& line = lines[i];
    size_t colon = line.find(':');
    std::string key = line.substr(0, colon);
    std::string value = line.substr(colon + 1, line.size() - colon);
    /* left trim value */
    value = value.substr(value.find_first_not_of(" \t\r\n\f\v"));

    /* return status code */
    if (key.compare("Status") == 0) {
      size_t space = value.find(' ');
      if (space == std::string::npos) {
        status = ft_atoi(value.c_str());
      } else {
        status = ft_atoi(value.substr(0, space).c_str());
      }
    } else {
      parse_map[key] = value;
    }
  }
}


static void saveMap
(Header& header, std::map<std::string, std::string>& parse_map) {
  std::map<std::string, std::string>::iterator it;

  it = parse_map.begin();
  while (it != parse_map.end()) {
    header[it->first] = it->second;
    it++;
  }
}

/*
 * process
 * description: add until header. when found end of header, parse and send it
 */

int CgiBody::recv(int fd) {
  char*   new_line;
  int     status;
  int     n_read;
  std::map<std::string, std::string>  parse_map;

  /* read from pipe */
  if ((n_read = read(fd, _read_buf, BUFSIZE)) < 0) {
    log("[CgiBody::recv]: read failed");
    return -1;
  }
  _read_buf[n_read] = '\0';
  
  log("[CgiBody::recv] read (%d bytes, total %d)\n", n_read, (int)_raw_body.length());
  /* body is closed */
  if (n_read == 0 && !_is_body_closed) {
    _is_body_closed = true;
    parse(status, parse_map);
    log("[CgiBody::recv] make header\n");
    *_header = new Header(status);
    saveMap(**_header, parse_map);
    (**_header)["Content-Length"] = std::to_string(_raw_body.length());
    log("[CgiBody::recv] Content-Length = %lu\n", _raw_body.length());
    return n_read;
  }

  /* if header is closed, just add body */
  if (_is_header_closed) {
    _raw_body += std::string(_read_buf, n_read);
  } else {
    /* TODO: loigical error - can't properly detect empty new line */
    if ((new_line = ft_strnstr(_read_buf, "\r\n\r\n", n_read)) == NULL) {
      _raw_header += std::string(_buf, n_read);
    } else {
      /* header is closed */
      *new_line = '\0';
      _raw_header += _read_buf;
      _is_header_closed = true;
      _raw_body += std::string(new_line + 4, n_read - (new_line + 4 - _read_buf));
    }
  }
  return n_read;
}

int CgiBody::send(int fd) {
  int n_written = 0;
  log("[CgiBody::send] n_sent = %d\n", _pos);
  if (!_is_body_closed)
    return 0;

 if (_pos < (size_t)ft_atoi((**_header)["Content-Length"].c_str())) {
      int size = std::min(BUFSIZE, (int)_raw_body.size());
      if ((n_written = ::write(fd, _raw_body.c_str() + _pos, size)) < 0) {
        log("[CgiBody::send]: write failed");
      }
      _pos += n_written;
  }
  return n_written;
}