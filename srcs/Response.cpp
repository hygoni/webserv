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
#include "Fd.hpp"

Response::Response(Client& client) : _body(false) {
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
    std::string header = _header.toString();
    _is_header_sent = true;
    std::string dummy = "HTTP/1.1 200 OK\r\n"
                      "Date: Sat, 28 Nov 2009 04:36:25 GMT\r\n"
                      "Server: LiteSpeed\r\n"
                      "Content-Length: 15\r\n\r\n";
    return ::send(fd,  dummy.c_str(), dummy.size(), 0);
  } else {
    return _body.send(fd);
  }
}

/* generate response with specific status */
Response::Response(int status) : _body(false) {
  _header = Header(status);
  _header["Content-Length"] = "0";
}

void Response::processCgi
(Client& client, Location const& location) {
  std::map<std::string, std::string> env_map;
  run_cgi(client, location.getCgiPath().c_str(), env_map, client.getRequestPipe()[0]);
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
    if (client.getRequest()->getTarget().find(location.getPath()) == 0) {
      std::vector<std::string> allowed = location.getAllowedMethod();
      /* Method Not Allowed */
      if (std::find(allowed.begin(), allowed.end(), client.getRequest()->getMethod())
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

void Response::setCgiPid(pid_t pid) {
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
(Client& client, Location const& location) {
  struct stat   buf;
  std::string   path;
  int           ret;
  
  path = location.getRoot() + client.getRequest()->getTarget();
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
  _header = Header(200);
  _header["Content-Length"] = "0";
  }
}

/*
 * processHeadMethod
 * processes HEAD request
 * main difference between GET and HEAD is,
 * there must be no content in HEAD's response
 */
void Response::processHeadMethod
(Client& client, Location const& location) {
  struct stat   buf;
  std::string   path;
  int           ret;
  
  path = location.getRoot() + client.getRequest()->getHeader()->getTarget();
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
  _header = Header(200);
  _header["Content-Length"] = "0";
}

void Response::processPostMethod
(Client& client, Location const& location) {
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
    path = location.getRoot() + client.getRequest()->getTarget();
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
    _header = Header(200);
    _header["Content-Length"] = "0";
  }
}
