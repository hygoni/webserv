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
#include "Client.hpp"

Response::Response
(Client const& client) : _offset(0) {
  if (process(client)) {
      return ;
  }
  /* no matching server found */
  *this = Response(400);
}

int Response::recv(int fd) {
  return _body.recv(fd);
}

int Response::send(int fd) {
  if (_is_header_sent == false) {
    return _header.send(fd);
  } else {
    return _body.send(fd);
  }
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
(Client& client, Location const& location) {
  std::map<std::string, std::string> env_map;
  run_cgi(*this, location.getCgiPath().c_str(), env_map, client.getRequestPipe[0]);
}

bool Response::process
(Client& client) {
  std::vector<Location>::const_iterator it;
  std::vector<Location>::const_iterator ite;

  it = client.getLocations().begin();
  ite = client.getLocations().end();
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
        processCgi(client, location);
      } else {
        /* process non-CGI */
        processByMethod(client, location);
      }
      return true;
    }
    it++;
  }
  return false;
}

void Response::setCgiPid(int pid) {
  _cgi_pid = pid;
}

/*
 * TODO: what about CGI?
 */
void Response::processByMethod
(Client& client, Location const& location) {
  Header& header = *(client.getRequest()->getHeader());
  if (header.getMethod().compare("GET") == 0) {
    processGetMethod(client, location);
  } else if (header.getMethod().compare("HEAD") == 0) {
    processHeadMethod(client, location);
  } else if (header.getMethod().compare("POST") == 0) {
    processPostMethod(client, location);
  } else if (header.getMethod().compare("PUT") == 0) {
  } else if (header.getMethod().compare("DELETE") == 0) {
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
(Request const& client, Location const& location) {
  struct stat   buf;
  std::string   path;
  int           ret;
  
  path = location.getRoot() + client->getRequest()->getHeader()->getTarget();
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

  this->header->addStatusLine(200);
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
