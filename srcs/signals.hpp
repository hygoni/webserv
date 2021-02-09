#ifndef SRCS_SIGNALS_HPP_
#define SRCS_SIGNALS_HPP_

void sigpipe_handler(int sig);
void sigchld_handler(int sig);
void exit_handler(int sig);

#endif  // SRCS_SIGNALS_HPP_
