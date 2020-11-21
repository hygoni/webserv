/* Copyright 2020 hyeyoo */

#define BUFSIZE 8192

#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "Message.hpp"

Response::Response(Request const& request) : _offset(0) {
  std::vector<Server>::const_iterator it;
  std::vector<Server>::const_iterator ite;
  Config*                             config;
  
  config = Config::getInstance();
  if (config == nullptr)
    throw std::exception();

  it = config->getServers().begin();
  ite = config->getServers().end();

  /* process request */
  while (it != ite) {
    Server const& server = *it;
    if (process(request, server)) {
        return ;
    }
    it++;
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

bool Response::process(Request const& request, Server const& server) {
  std::vector<Location>::const_iterator it;
  std::vector<Location>::const_iterator ite;

  it = server.getLocations().begin();
  ite = server.getLocations().end();
  /* find matching location */
  while (it != ite) {
    Location const& location = *it;
    /* TODO: add CGI */
    if (request.getTarget().find(location.getPath()) == 0) {
      std::vector<std::string> allowed = location.getAllowedMethod();
      /* Method Not Allowed */
      if (std::find(allowed.begin(), allowed.end(), request.getMethod())
          == allowed.end() && allowed.size() > 0) {
        *this = Response(405);
      } else {
        processByMethod(request, location);
      }
      return true;
    }
    it++;
  }
  return false;
}

void Response::processByMethod
(Request const& request, Location const& location) {
  if (request.getMethod().compare("GET") == 0) {
    processGETMethod(request, location);
  } else if (request.getMethod().compare("HEAD") == 0) {
    processHEADMethod(request, location);
  } else if (request.getMethod().compare("POST") == 0) {
    processPOSTMethod(request, location);
  } else if (request.getMethod().compare("PUT") == 0) {
  } else if (request.getMethod().compare("DELETE") == 0) {
  } else {
    /* Not Implemented */
    *this = Response(501);
  }
}

int readContent(std::string const& path, std::string& content) {
  int fd;
  int ret;
  char buf[BUFSIZE + 1];

  fd = open(path.c_str(), O_RDONLY);
  if (fd < 0)
    return fd;
  while ((ret = read(fd, buf, BUFSIZE)) > 0) {
    buf[ret] = '\0';
    content += std::string(buf);
  }
  if (ret < 0)
    return ret;
  return 0;
}

void Response::processGETMethod
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

  std::string content;
  if (readContent(path, content) < 0) {
    /* Internal Server Error */
    *this = Response(500);
  } else {
    this->addStatusLine(200);
    this->addHeader("Content-Length", std::to_string(content.size()));
    this->endHeader();
    this->addBody(content);
  }
}

/*
 * processHEADMethod
 * processes HEAD request
 * main difference between GET and HEAD is,
 * there must be no content in HEAD's response
 */
void Response::processHEADMethod
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

void Response::processPOSTMethod
(Request const& request, Location const& location) {
  struct stat   buf;
  std::string   path;
  int           ret;
  
  /* POST is allowed only for CGI requests
   * TODO: add CGI
   * */
  if (location.getCGIPath().empty()) {
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
