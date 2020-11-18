/* Copyright 2020 hyeyoo */

#include "Message.hpp"

Message* Message::_instance = nullptr;

Message::Message() {
  this->_msg[100] = "Continue";
  this->_msg[101] = "Switching Protocols";
  this->_msg[103] = "Checkpoint";
  this->_msg[200] = "OK";
  this->_msg[201] = "Created";
  this->_msg[202] = "Accepted";
  this->_msg[203] = "Non-Authoritative Information";
  this->_msg[204] = "No Content";
  this->_msg[205] = "Reset Content";
  this->_msg[206] = "Partial Content";
  this->_msg[300] = "Multiple Choices";
  this->_msg[301] = "Moved Permanently";
  this->_msg[302] = "Found";
  this->_msg[303] = "See Other";
  this->_msg[304] = "Not Modified";
  this->_msg[306] = "Switch Proxy";
  this->_msg[307] = "Temporary Redirect";
  this->_msg[308] = "Resume Incomplete";
  this->_msg[400] = "Bad Request";
  this->_msg[401] = "Unauthorized";
  this->_msg[402] = "Payment Required";
  this->_msg[403] = "Forbidden";
  this->_msg[404] = "Not Found";
  this->_msg[405] = "Method Not Allowed";
  this->_msg[406] = "Not Acceptable";
  this->_msg[407] = "Proxy Authentication Required";
  this->_msg[408] = "Request Timeout";
  this->_msg[409] = "Conflict";
  this->_msg[410] = "Gone";
  this->_msg[411] = "Length Required";
  this->_msg[412] = "Precondition Failed";
  this->_msg[413] = "Request Entity Too Large";
  this->_msg[414] = "Request-URI Too Long";
  this->_msg[415] = "Unsupported Media Type";
  this->_msg[416] = "Requested Range Not Satisfiable";
  this->_msg[417] = "Expectation Failed";
  this->_msg[500] = "Internal Server Error";
  this->_msg[501] = "Not Implemented";
  this->_msg[502] = "Bad Gateway";
  this->_msg[503] = "Service Unavailable";
  this->_msg[504] = "Gateway Timeout";
  this->_msg[505] = "HTTP Version Not Supported";
  this->_msg[511] = "Network Authentication Required";
}

Message* Message::getInstance() {
  if (Message::_instance == nullptr)
    Message::_instance = new Message();
  return Message::_instance;
}

std::string const& Message::getMessage(int status) {
  if (this->_msg.find(status) == this->_msg.end())
    throw std::exception();
  else
    return this->_msg[status];
}
