#include "HttpException.hpp"

HttpException::HttpException(int status) {
  this->_status = status;
}

int HttpException::getStatus() const {
  return this->_status;
}