#include <string>
#include "Request.hpp"

std::string decodeBase64(char* src) {
  std::string result;
  int   i = 0;
  int   remainder = 0;

  while (src[i] != '\0') {
    result[i] += src[i] >> (2 + remainder);
    remainder = (remainder + 2) % 6;

    result += '\x00';
    result[i + 1] = src[i] << (6 - remainder);
    i++;
  }
  return result;
}

bool  authLogin(std::string userName, std::string password) {
  
}

bool  auth(Header header) {
  std::string credentials, scheme, token;
  size_t i;

  credentials = header["Authorization"];
  i = credentials.find(' ');
  if (i == std::string::npos)
    return false;
  scheme = credentials.substr(0, i);
  if (scheme != "Basic")
    return false;
  // validate login password , decode 
  return true;
}
