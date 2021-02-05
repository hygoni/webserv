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
#include "CgiBody.hpp"
#include "debug.hpp"
#include "utils.hpp"

char *Response::_buf = (char*)malloc(sizeof(char) * (BUFSIZE + 1));

Response::Response(Client& client) : _client(client) {
  debug_printf("[Response::Response(Client&)]\n");
  _header = NULL;
  _body = NULL;
  _is_header_sent = false;
  _is_cgi = false;
  _file_fd = -1;
  _pos = 0;
  _pos_cgi = 0;
  _cgi = NULL;
  // Fd::setWfd(client.getFd());
  process(client);
}

Response::Response(Client& client, int status) : _client(client) {
  debug_printf("[Response::Response(Client&)]\n");
  _header = NULL;
  _body = NULL;
  _is_header_sent = false;
  _is_cgi = false;
  _file_fd = -1;
  _pos = 0;
  _pos_cgi = 0;
  _cgi = NULL;
  // Fd::setWfd(client.getFd());
  setStatus(status);
}

Response::~Response() {
  debug_printf("[Response::~Response]\n");
  if (_header != NULL)
    delete _header;
  if (_body != NULL)
    delete _body;
  if (_cgi != NULL)
    delete _cgi;
  Fd::close(_file_fd);
}

// fd: file fd or
int Response::recv(const fd_set *rfds, const fd_set *wfds) {
  int           n_read;
  int           n_write;

  if (_file_fd != -1) {
    if (Fd::isSet(_file_fd, rfds)) {
      if ((n_read = read(_file_fd, _buf, BUFSIZE)) < 0)
        return n_read;
      _body->addBody(std::string(_buf, n_read));
      /* for PUT request, add request body to response body */
    } else if (_client.getRequest()->getMethod() == "PUT" &&
              _client.getRequest()->getBody()->toString().length() > _body->toString().length()) {
      _body->addBody(_client.getRequest()->getBody()->toString());
    }
    _client.updateTime();

  } else if (_is_cgi) {
    if (Fd::isSet(_client.getResponsePipe()[0], rfds)) {
      if ((n_read = read(_client.getResponsePipe()[0], _buf, BUFSIZE)) < 0)
        return n_read;
      _body->addBody(std::string(_buf, n_read));
      _client.updateTime();
    }
    if (Fd::isSet(_client.getRequestPipe()[1], wfds)) {
      std::string const& req_body = _client.getRequest()->getBody()->toString();
      int len = std::min((int)req_body.length() - _pos_cgi, BUFSIZE);
      if ((n_write = write(_client.getRequestPipe()[1], req_body.c_str() + _pos_cgi, len)) < 0)
        return n_write;
      _pos_cgi += n_write;
      /* sent all request body */
      if (_pos_cgi == (int)req_body.length()) {
        Fd::close(_client.getRequestPipe()[1]);
      }
      _client.updateTime();
      return n_write;
    }
  }
  return 0;
}

int Response::send(int fd) {
  int ret = 0;
  /* write body to _file_fd when PUT response, what about PUT && CGI? */

  _client.updateTime();
  if (_is_header_sent == false) {
    if (_header != NULL) {
      std::string const& header = _header->toString();
      if ((ret = write(fd,  header.c_str() + _pos, header.size() - _pos)) < 0)
        return ret;
      _pos += ret;
      if (_pos == (int)header.length()) {
        _pos = 0;
        _is_header_sent = true;
      }
    }
    if (_header != NULL && _header->getStatus() / 100 != 2 && _body == NULL)
      return -1;
    else
      return 1;
  } else if (_client.getRequest() != NULL) {
    if (_body != NULL) {
      int body_fd = (_client.getRequest()->getMethod() == "PUT" && !_is_cgi) ? _file_fd : fd;
      int len = std::min((int)_body->toString().length() - _pos, BUFSIZE);
      if ((ret = ::write(body_fd, _body->toString().c_str() + _pos, len)) < 0)
        return ret;
      _pos += ret;
      /* all body is sent */
      if (!_body->isFinished())
        return 1;
        /* all body is sent */
      else if (_pos == (int)_body->toString().length())
        return -1;
    } else { /* response has no body */
        return -1;
    }
  }
  return ret;
}

/* default headers are here */
Header* Response::initHeader(int status) const {
  Header* header = new Header(status);
  (*header)["Content-Length"] = "0";

  /* set date */
  struct timeval raw;
  struct tm time_info;
  char buf[1024];
  gettimeofday(&raw, NULL);
  set_tm(raw.tv_sec, &time_info);
  strftime(buf, 1024, "%a, %d %b %Y %H:%M:%S GMT", &time_info);
  (*header)["Date"] = buf;

  /* set allow header if status is 405 Method Not Allowed */
  if (status == 405) {
    std::string allowed_method;
    std::vector<std::string> const& allowed = _client.getLocation()->getAllowedMethod();
    for (size_t i = 0; i < allowed.size(); i++) {
      allowed_method.append(allowed[i]);
      if (i + 1 != allowed.size()) {
        allowed_method.append(", ");
      }
    }
    (*header)["Allowed"] = allowed_method;
  }

  if (status / 100 != 2 || (*_client.getRequest()->getHeader())["Connection"] == "close") {
    (*header)["Connection"] = "close";
  } else {
    (*header)["Connection"] = "keep-alive";
  }
  return header;
}

void Response::setStatus(int status) {
  debug_printf("[Response::Response(int)]");
  if (_header != NULL)
    delete _header;
  _header = initHeader(status);
}

void Response::processCgi
(Client& client) {
  _cgi = new Cgi(client);
  _cgi->run();
}

void Response::process
(Client& client) {
  Location const& location = *client.getLocation();
  std::vector<std::string> allowed = location.getAllowedMethod();
  /* Method Not Allowed */
  if (std::find(allowed.begin(), allowed.end(), client.getRequest()->getMethod())
      == allowed.end() && allowed.size() > 0) {
    setStatus(405);
  } else if (client.isCgi()) {
    _is_cgi = true;
    /* process CGI */
    _body = new CgiBody(&_header);
    processCgi(client);
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
    std::string path = location.getRoot() + "/" + client.getRequest()->getTarget().substr(location.getPath().length());
    processByMethod(client, location, path);
  }
}

void Response::setCgiPid(pid_t pid) {
  _cgi_pid = pid;
}

void Response::processByMethod
(Client& client, Location const& location, std::string const& path) {
  Header& header = *(client.getRequest()->getHeader());
  if (header.getMethod().compare("GET") == 0) {
    processGetMethod(client, location, path);
  } else if (header.getMethod().compare("HEAD") == 0) {
    processHeadMethod(path);
  } else if (header.getMethod().compare("POST") == 0) {
    processGetMethod(client, location, path);
  } else if (header.getMethod().compare("PUT") == 0) {
    processPutMethod(client, location);
  } else if (header.getMethod().compare("DELETE") == 0) {
  } else {
    /* Not Implemented */
   setStatus(501);
  }
}

void Response::processPutMethod
(Client& client, Location const& location) {
  struct stat   buf;
  std::string   path;
  int           ret;

  path = location.getRoot() + "/" + client.getRequest()->getTarget().substr(location.getPath().length());
  ret = stat(path.c_str(), &buf);

  if (ret < 0) {
    if (errno == EACCES) {
      /* Forbidden */
      setStatus(403);
    } else if (errno == ENOENT) {
      /* 201 Created */
      setStatus(201);
      _file_fd = open(path.c_str(), O_RDWR | O_CREAT, 0644);
      (*_header)["Location"] = client.getRequest()->getTarget();
      if (_file_fd < 0)
        throw "[Response::processPutMethod] open failed";
      Fd::setWfd(_file_fd);
      _body = new Body(0);
    } else {
      /* Internal Server Error */
      setStatus(500);
    }
  } else {
    /* 200 OK */
    _file_fd = open(path.c_str(), O_RDWR | O_CREAT, 0644);
    if (_file_fd < 0)
      throw "[Response::processPutMethod] open failed";
    Fd::setWfd(_file_fd);
    _body = new Body(0);
    setStatus(200);
  }
}

void Response::processDefaultErrorPage(int status) {
  std::string html;

  std::string msg = Message::getInstance()->getMessage(status);
  html = "<h1>" + std::to_string(status) + " " + msg + "</h1>";
  debug_printf("[Response::processDefaultErrorPage] %s\n", html.c_str());
  (*_header)["Content-Length"] = std::to_string(html.length());
  _body = new Body(html.length());
  _body->addBody(html);
}

void Response::processDirectoryListing
(Client& client, std::string const& path) {
  DIR *dir;
  struct dirent *ent;
  std::string html;

  if ((dir = opendir(path.c_str())) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir(dir)) != NULL) {
      html += "<p><a href=\"http://" + client.getServer().getServerName() + client.getRequest()->getTarget() + "/" + ent->d_name + "\">";
      html += std::string(ent->d_name) + "</a></p>";
    }
    closedir(dir);
  } else {
    /* could not open directory */
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
  debug_printf("[Response::processDListing] %s\n", html.c_str());
  (*_header)["Content-Length"] = std::to_string(html.length());
  _body = new Body(html.length());
  _body->addBody(html);
}

void Response::processGetMethod
(Client& client, Location const& location, std::string path) {
  struct stat   buf;
  int           ret;

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
    processDefaultErrorPage(_header->getStatus());
    return ;
  } else if (S_ISDIR(buf.st_mode)) {
    if (location.getDirectoryListing()) {
      processDirectoryListing(client, path);
      return ;
    }
    for (size_t i = 0; i < location.getIndex().size(); i++) {
      ret = stat((path + "/" + location.getIndex()[i]).c_str(), &buf);
      if (ret < 0 && errno == ENOENT)
        continue;
      else {
        path = path + "/" + location.getIndex()[i];
        break;
      }
    }
  }

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
    processDefaultErrorPage(_header->getStatus());
    return ;
  }
  _body = new Body(buf.st_size);
  setStatus(200);
  (*_header)["Content-Length"] = std::to_string(buf.st_size);
  _file_fd = open(path.c_str(), O_RDONLY);
  if (_file_fd < 0) {
    throw "[Response::processGetMethod] File Exception";
  }
  Fd::setRfd(_file_fd);
}

/*
 * processHeadMethod
 * processes HEAD request
 * main difference between GET and HEAD is,
 * there must be no content in HEAD's response
 */
void Response::processHeadMethod
(std::string const& path) {
  struct stat   buf;
  int           ret;

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

bool Response::isCgi() const {
  return _is_cgi;
}

Header* Response::getHeader() {
  return _header;
}

int     Response::getFileFd() const {
  return _file_fd;
}

Cgi     *Response::getCgi() {
  return _cgi;
}
