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

Response::Response(Client& client) : _client(client) {  
  std::cout << "[Response::Response(Client&)]" << std::endl;
  _header = NULL;
  _body = NULL;
  _is_header_sent = false;
  _is_Cgi = false;
  _file_fd = -1;
  _n_sent = 0;

  Fd::setWfd(client.getFd());
  process(client);
}

Response::Response(Client& client, int status) : _client(client) {  
  std::cout << "[Response::Response(Client&)]" << std::endl;
  _header = NULL;
  _body = NULL;
  _is_header_sent = false;
  _is_Cgi = false;
  _file_fd = -1;
  _n_sent = 0;

  Fd::setWfd(client.getFd());
  setStatus(status);
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
  if (_file_fd != -1)
    close(_file_fd);
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
  int ret = 0;

  if (_is_header_sent == false) {
    if (_header != NULL) {
      std::string header = _header->toString();
      _is_header_sent = true;
      return ::send(fd,  header.c_str(), header.size(), 0);
    } else {
      return 0;
    }
  } else {
    if (_body != NULL) {
      if (_is_Cgi) {
        ret = _body->send(fd);
        /* for PUT request, request body (saved in body) is written to _file_fd */
      } else if (_client.getRequest()->getMethod() == "PUT") {
        int client_read_fd = _client.getRequestPipe()[0];
        Fd::setRfd(client_read_fd);
        /* read from read end of request pipe */
        if (Fd::isSet(client_read_fd, *Fd::rfds)) {
          _body->recv(client_read_fd);
        }
        ret = _body->send(_file_fd);
      } else {
        if (_file_fd != -1)
          _body->recv(_file_fd);
        ret = _body->send(fd);
      }
    }
    _n_sent += ret;
    if ((_client.getRequest() != NULL && _client.getRequest()->getMethod() == "PUT" && _n_sent == _client.getRequest()->getContentLength()) ||
    _n_sent == _header->getContentLength() ||
    _body == NULL) {
      log("[Response::send] close, clear %d\n", fd);
      close(fd);
      return -1;
    }
    if (ret < 0) {
      log("[Response::send] ret is %d\n", ret);
      log("[Response::send] %s\n", strerror(errno));
    }
    return ret;
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
  Cgi cgi(client);
  cgi.run();
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
    _is_Cgi = true;
    /* process CGI */
    bool is_transfer_encoding = client.getRequest()->isChunked();
    _body = new CgiBody(&_header);
    if (!is_transfer_encoding) {
      processCgi(client, location);
    }
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
    processHeadMethod(client, location, path);
  } else if (header.getMethod().compare("POST") == 0) {
    processPostMethod();
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

  path = location.getRoot() + client.getRequest()->getTarget();
  ret = stat(path.c_str(), &buf);

  if (ret < 0) {
    if (errno == EACCES) {
      /* Forbidden */
      setStatus(403);
    } else if (errno == ENOENT) {
      /* 201 Created */
      setStatus(201);
      _file_fd = open(path.c_str(), O_RDWR | O_CREAT, 0644);
      if (_file_fd < 0)
        throw "[Response::processPutMethod] open failed";
      _body = new Body();
    } else {
      /* Internal Server Error */
      setStatus(500);
    }
  } else {
    /* 200 OK */
    _file_fd = open(path.c_str(), O_RDWR | O_CREAT, 0644);
    if (_file_fd < 0)
      throw "[Response::processPutMethod] open failed";
    _body = new Body();
    setStatus(200);
  }
}

void Response::processDirectoryListing
(Client& client, Location const& location, std::string const& path) {
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
  log("[Response::processDListing] %s\n", html.c_str());
  (*_header)["Content-Length"] = std::to_string(html.length());
  _body = new Body(html);
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
    return ;
  } else if (S_ISDIR(buf.st_mode)) {
    if (location.getDirectoryListing()) {
      return processDirectoryListing(client, location, path);
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
    return ;
  }
  _body = new Body();
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
(Client& client, Location const& location, std::string const& path) {
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
  return _is_Cgi;
}

Header* Response::getHeader() {
  return _header;
}
