/* Copyright 2020 hyeyoo */

#include <stdlib.h>
#include <exception>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include "Buffer.hpp"
#include "libft.h"
#include "Fd.hpp"
#include "Header.hpp"
#include "CgiBuffer.hpp"

CgiBuffer::CgiBuffer(size_t size) : Buffer(size) {
  _is_header_closed = false;
  _is_body_closed = false;
  _is_header_sent = false;
  _header = NULL;
  _n_sent = 0;
}

CgiBuffer::~CgiBuffer() {
  if (_header != NULL)
    delete _header;
}


void CgiBuffer::parse
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


void saveMap
(Header& header, std::map<std::string, std::string>& parse_map) {
  std::map<std::string, std::string>::iterator it;

  it = parse_map.begin();
  while (it != parse_map.end()) {
    header[it->first] = it->second;
    std::cout << "Key : " << it->first << " Value : " << it->second << std::endl;
    it++;
  }
}

/*
 * process
 * description: add until header. when found end of header, parse and send it
 */

int CgiBuffer::recv(int fd) {
  int     n_sent;
  char    *new_line;
  int     status;
  int     n_read;
  std::map<std::string, std::string>  parse_map;

  /* read from pipe */
  if ((n_read = read(fd, _buf, _size)) < 0)
    throw "[CgiBuffer::send]: read failed";
  _buf[n_read] = '\0';
  _len = n_read;

  /* body is closed */
  if (n_read == 0) {
    _is_body_closed = true;
    (*_header)["Content-Length"] = std::to_string(_raw_body.size());
    return n_read;
  }

  /* if header is closed, just add body */
  if (_is_header_closed) {
    _raw_body += _buf;
  } else {
    /* TODO: loigical error - can't properly detect empty new line */
    if ((new_line = ft_strnstr(_buf, "\r\n\r\n", _len)) == NULL) {
      _raw_header += _buf;
    } else {
      /* header is closed */
      *new_line = '\0';
      _raw_header += _buf;
      parse(status, parse_map);
      _header = new Header(status);
      saveMap(*_header, parse_map);
      _is_header_closed = true;
      _raw_body += (new_line + 4);
    }
  }
  return n_sent;
}

int CgiBuffer::send(int fd) {
  int n_written;

  if (!_is_body_closed)
    return 0;
  
 if (!_is_header_sent) {
    /* TODO: Warning: is header always can be sent by writing once? */
    if ((n_written = ::write(fd, _header->toString().c_str(), _header->toString().size())) < 0)
      throw "[Buffer::send]: write failed";
    _is_header_sent = true;
    std::cout << "Sent: " << _header->toString();
  } else if (_n_sent < (size_t)ft_atoi((*_header)["Content-Length"].c_str())) {
      size_t size = std::min(_size, _raw_body.size());
      if ((n_written = ::write(fd, _raw_body.c_str(), size)) < 0)
        throw "[Buffer::send]: write failed";
      std::cout << "Sent: " << _raw_body.substr(0, n_written);
      _raw_body = _raw_body.substr(n_written);
      _n_sent += n_written;
  }
  std::cout << "n_sent is : " << _n_sent << ", CL : " << (*_header)["Content-Length"] << std::endl;
  return n_written;
}