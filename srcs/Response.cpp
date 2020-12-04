/* Copyright 2020 hyeyoo */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "Message.hpp"
#include "CGI.hpp"

Response::Response
(Request const& request, Server const& server) : _offset(0) {
  if (process(request, server)) {
      return ;
  }
  /* no matching server found */
  *this = Response(400);
}

void Response::addStatusLine(int status) {
  Message* msg = Message::getInstance();
  this->_response += "HTTP/1.1 " + std::to_string(status) + " ";
  this->_response += msg->getMessage(status) + "\r\n";
}

void Response::addHeader(std::string key, std::string value) {
  this->_response += (key + ": " + value + "\r\n");
}

void Response::endHeader() {
  this->_response += "\r\n";
}

void Response::addBody(std::string const& content) {
  this->_response += content;
}

/* generate response with specific status */
Response::Response(int status) : _offset(0) {
  /* status line */
  this->addStatusLine(status);
  this->addHeader("Content-Length", "0");
  this->endHeader();
}

void Response::processCgi
(Request const& request, Location const& location) {
  (void)request;
  int body_fd[2];
  pipe(body_fd);
  std::map<std::string, std::string> env_map;
  setBodyWriteFd(body_fd[1]);
  run_cgi(*this, location.getCgiPath().c_str(), env_map, body_fd[0]);
}

bool Response::process(Request const& request, Server const& server) {
  std::vector<Location>::const_iterator it;
  std::vector<Location>::const_iterator ite;

  it = server.getLocations().begin();
  ite = server.getLocations().end();
  /* find matching location */
  while (it != ite) {
    Location const& location = *it;
    if (request.getTarget().find(location.getPath()) == 0) {
      std::vector<std::string> allowed = location.getAllowedMethod();
      /* Method Not Allowed */
      if (std::find(allowed.begin(), allowed.end(), request.getMethod())
          == allowed.end() && allowed.size() > 0) {
        *this = Response(405);
      } else if (!location.getCgiPath().empty()) {
        /* process CGI */
        processCgi(request, location);
      } else {
        /* process non-CGI */
        processByMethod(request, location);
      }
      return true;
    }
    it++;
  }
  return false;
}

int Response::writeBody() {
  int ret = write(_body_write_fd, _body_buf, _body_length);
  _body_length = 0;
  return ret;
}

int Response::closeBody() {
  return close(_body_write_fd);
}

char* Response::getBodyBuffer() {
  return _body_buf;
}

char* Response::getResponseBuffer() {
  return _response_buf;
}

size_t Response::getBodyLength() const {
  return _body_length;
}


size_t Response::getResponseLength() const {
  return _response_length;
}

void Response::setBodyLength(size_t len) {
  _body_length = len;
}

bool Response::isBodyReady() const {
  return (_body_length > 0);
}

void Response::setCgiPid(int pid) {
  _cgi_pid = pid;
}

void Response::setResponseReadFd(int fd) {
  _response_read_fd = fd;
}

void Response::setBodyWriteFd(int fd) {
  _body_write_fd = fd;
}

int Response::readResponse() {
  return (_response_length = read(_response_read_fd, _response_buf, BUFSIZE));
}


/*
 * TODO: what about CGI?
 */
void Response::processByMethod
(Request const& request, Location const& location) {
  if (request.getMethod().compare("GET") == 0) {
    processGetMethod(request, location);
  } else if (request.getMethod().compare("HEAD") == 0) {
    processHeadMethod(request, location);
  } else if (request.getMethod().compare("POST") == 0) {
    processPostMethod(request, location);
  } else if (request.getMethod().compare("PUT") == 0) {
  } else if (request.getMethod().compare("DELETE") == 0) {
  } else {
    /* Not Implemented */
    *this = Response(501);
  }
}

void Response::processGetMethod
(Request const& request, Location const& location) {
  struct stat   buf;
  std::string   path;
  int           ret;
  
  path = location.getRoot() + request.getTarget();
  ret = stat(path.c_str(), &buf);
  if (ret < 0) {
    if (errno == EACCES) {
      /* Forbidden */
      *this = Response(403);
    } else if (errno == ENOENT) {
      /* Not Found */
      *this = Response(404);
    } else {
      /* Internal Server Error */
      *this = Response(500);
    }
    return ;
  }

  if (false) {
    /* Internal Server Error */
    *this = Response(500);
  } else {
    this->addStatusLine(200);
    this->addHeader("Content-Length", "0");
    this->endHeader();
  }
}

/*
 * processHeadMethod
 * processes HEAD request
 * main difference between GET and HEAD is,
 * there must be no content in HEAD's response
 */
void Response::processHeadMethod
(Request const& request, Location const& location) {
  struct stat   buf;
  std::string   path;
  int           ret;
  
  path = location.getRoot() + request.getTarget();
  ret = stat(path.c_str(), &buf);
  if (ret < 0) {
    if (errno == EACCES) {
      /* Forbidden */
      *this = Response(403);
    } else if (errno == ENOENT) {
      /* Not Found */
      *this = Response(404);
    } else {
      /* Internal Server Error */
      *this = Response(500);
    }
    return ;
  }
  this->addStatusLine(200);
  this->addHeader("Content-Length", "0");
  this->endHeader();
}

void Response::processPostMethod
(Request const& request, Location const& location) {
  struct stat   buf;
  std::string   path;
  int           ret;
  
  /* POST is allowed only for CGI requests
   * TODO: add CGI
   * */
  if (location.getCgiPath().empty()) {
    /* Method Not Allowed */
    *this = Response(405);
  } else {
    path = location.getRoot() + request.getTarget();
    ret = stat(path.c_str(), &buf);
    if (ret < 0) {
      if (errno == EACCES) {
        /* Forbidden */
        *this = Response(403);
      } else if (errno == ENOENT) {
        /* Not Found */
        *this = Response(404);
      } else {
        /* Internal Server Error */
        *this = Response(500);
      }
      return ;
    }
    this->addStatusLine(200);
    this->addHeader("Content-Length", "0");
    this->endHeader();
  }
}

/* 
 * send
 * return value ( = val)
 * if val >= 0, it is bytes left to send
 * if val < 0, error occurred while sending
 */
int Response::send(int fd) {
  std::string buf;
  int ret;

  buf = this->_response.substr(this->_offset, this->_offset + BUFSIZE);
  ret = ::send(fd, buf.c_str(), BUFSIZE, 0);
  if (ret < 0)
  return ret;
  /* advance offset */
  this->_offset += ret;
  return (this->_response.size() - this->_offset);
}
