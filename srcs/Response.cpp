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
#include "CgiBuffer.hpp"

Response::Response(Client& client) {  
  std::cout << "[Response::Response(Client&)]" << std::endl;
  _header = NULL;
  _body = NULL;
  _is_header_sent = false;
  _is_Cgi = false;
  _file_fd = -1;

  Fd::setWfd(client.getFd());
  if (process(client)) {
      return ;
  }
  /* no matching server found */
  setStatus(400);
}

int Response::recv(int fd) {
  return _body->recv(fd);
}

Response::~Response() {
  std::cout << "[Response::~Response]" << std::endl;
  if (_header != NULL)
    delete _header;
  if (_body != NULL)
    delete _body;
}

int Response::send(int fd) {
  /* CGI middleware here? or body type?
    where to generate header?
    CGI? non-CGI?
    when CGI, header is created when all output of CGI is recieved.
    when non-CGI, header is created when opening file.
    where goes CGI middleware?

    when CGI, header is created in Body
    when non-CGI, header is immediately created
   */
  if (_header != NULL && _is_header_sent == false) {
    std::string header = _header->toString();
    _is_header_sent = true;
    /* why header isn't saved in buffer? */
    return ::send(fd,  header.c_str(), header.size(), 0);
  } else {
    if (_body == NULL)
      return 0;
    if (_is_Cgi) {
      return _body->send(fd);
    } else {
      _body->recv(_file_fd);
      return _body->send(fd);
    }
  }
}

void Response::setStatus(int status) {
  std::cout << "[Response::Response(int)]" << std::endl;
  if (_header != NULL)
    delete _header;
  _header = new Header(status);
}

void Response::processCgi
(Client& client, Location const& location) {
  Cgi cgi(client.getServer(), *client.getRequest()->getHeader());

  cgi.run(location.getCgiPath().c_str(), client.getRequestPipe(), client.getResponsePipe());
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
        setStatus(405);
      } else if (!location.getCgiPath().empty()) {
        _is_Cgi = true;
        /* process CGI */
        _body = new Body(new CgiBuffer(BUFSIZE), false);
        processCgi(client, location);
      } else {
        /* process non-CGI */
        /* TODO: when body exists in non-CGI response
        const Header* header = client.getRequest()->getHeader();
        if ((*_header)["Content-Length"].length() || (*_header)["Transfer-Encoding"].length()) {
          _body = new Body(false);
          pipe(client.getResponsePipe());
          Fd::setRfd(client.getResponsePipe()[0]);
        }
        */
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
    processPostMethod();
  } else if (header.getMethod().compare("PUT") == 0) {
  } else if (header.getMethod().compare("DELETE") == 0) {
  } else {
    /* Not Implemented */
   setStatus(501);
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
      setStatus(403);
    } else if (errno == ENOENT) {
      /* Not Found */
      setStatus(404);
    } else {
      /* Internal Server Error */
      setStatus(500);
    }
    return ;
  }
  _body = new Body(false);
  setStatus(200);
  (*_header)["Content-Length"] = std::to_string(buf.st_size);
  _file_fd = open(path.c_str(), O_RDONLY);
  if (_file_fd < 0) {
    throw "[Response::processGetMethod] File Exception";
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
      setStatus(403);
    } else if (errno == ENOENT) {
      /* Not Found */
      setStatus(404);
    } else {
      /* Internal Server Error */
      setStatus(500);
    }
    return ;
  }
  setStatus(200);
}

void Response::processPostMethod() {
  /* non-CGI POST is Method Not Allowed */
  setStatus(405);
}
