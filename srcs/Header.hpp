#ifndef SRCS_HEADER_HPP
# define SRCS_HEADER_HPP
# include <string>
# include <map>
# include <algorithm>
class Header {
private:
  std::map<std::string, std::string>  _items;
  
  std::string _method;
  std::string _target;
  std::string _version;

  int         _status;
  std::string _message;
public:
  Header();
  Header(std::string _method, std::string _target, std::string _version);
  Header(int status);
  std::string         toString() const;

  size_t              erase(const std::string& key);

  std::string&        operator[](const std::string& key);
  const std::string   operator[](const std::string& key) const;

  bool                isExist(const std::string& key) const;

  const std::string&  getMethod() const;
  const std::string&  getTarget() const;
  const std::string&  getVersion() const;
  int                 getStatus() const;
  const std::string&  getMessage() const;
};
#endif
