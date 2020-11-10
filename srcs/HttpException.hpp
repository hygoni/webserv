#ifndef HTTPEXCEPTION_HPP
# define HTTPEXCEPTION_HPP
# include <stdexcept>
class HttpException : public std::exception {
private:
  int _status;
  HttpException();
public:
  HttpException(int status);
  int getStatus() const;
  // virtual const char* what() const throw();
};
#endif