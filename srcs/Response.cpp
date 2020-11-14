/* Copyright 2020 hyeyoo */

#include <string>
#include "Response.hpp"

Response::Response(Request const& req) {
  (void)req;
}

void Response::send(int fd) {
  /* status line = HTTP-version SP status-code SP reason_phrase CRLF */
  std::string status_line;
  (void)fd;
  /* hmm...... how to determine status code? */


}
