#include <sys/types.h>
#include <sys/wait.h>
#include "signals.hpp"
#include "debug.hpp"

bool g_is_sigpipe = false;

void sigpipe_handler(int sig) {
  g_is_sigpipe = true;
  log("[SIGNAL] recieved SIGPIPE\n");
}

void sigchld_handler(int sig) {
  log("[SIGNAL] recieved SIGCHLD\n");
  pid_t pid;
  while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
    log("[CHILD] child (%d) exited \n", pid);
  }
}
