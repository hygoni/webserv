#ifndef REQUEST_HPP_
#define REQUEST_HPP_
#include <string>
/* Copyright 2020 hyeyoo, hyekim */
// ctx.state = ((i >= min) && (max == 0 ? true :  i <= max)); ?? repeat
class Request {
private:
  std::string _method;
  std::string _target;
  float       _version;

  Request();
  void  initMethod(Context& ctx);
  void  initTarget(Context& ctx);
  void  initVersion(Context& ctx);
public:
  Request(std::string raw);
};

#endif
