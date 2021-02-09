#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include "signals.hpp"
#include "debug.hpp"

bool g_is_sigpipe = false;

void sigpipe_handler(int sig) {
  (void)sig;
  g_is_sigpipe = true;
  debug_printf("[SIGNAL %d] recieved SIGPIPE\n", sig);
}

void sigchld_handler(int sig) {
  pid_t pid;

  (void)sig;
  debug_printf("[SIGNAL %d] recieved SIGCHLD\n", sig);
  while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
    debug_printf("[CHILD] child (%d) exited \n", pid);
  }
}

void exit_handler(int sig) {
  (void)sig;
  debug_printf("[SIGNAL %d] recieved\n", sig);
  /* kill all of child - it'll be freed by init process */
  
  signal(SIGQUIT, SIG_IGN);
  kill(-1, SIGQUIT);
  exit(EXIT_FAILURE);
}
