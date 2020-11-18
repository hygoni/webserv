#ifndef SRCS_MESSAGE_HPP_
#define SRCS_MESSAGE_HPP_

#include <string>
#include <map>

class Message {
 private:
   Message();
   std::map<int, std::string>   _msg;
   static Message*              _instance;
 public:
   Message*           getInstance();
   std::string const& getMessage(int status);
};

#endif  // SRCS_MESSAGE_HPP_
