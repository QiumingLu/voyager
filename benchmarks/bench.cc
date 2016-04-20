#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#include "mirants/core/dispatch.h"
#include "mirants/core/eventloop.h"
#include "mirants/util/scoped_ptr.h"

#define timersub(tvp, uvp, vvp)           \
  do {                \
    (vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;    \
    (vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec; \
    if ((vvp)->tv_usec < 0) {       \
      (vvp)->tv_sec--;        \
      (vvp)->tv_usec += 1000000;      \
    }             \
  } while (0)
#endif

EventLoop *eventloop;
scoped_array<scoped_ptr<Dispatch> > dispatches;

static int count, writes, fired;
static int *pipes;
static int num_pipes, num_active, num_writes;

void ReadCallback(int fd, int index) {
  char ch;
  count += static_cast<int>(::recv(fd, &ch, sizeof(ch), 0));
  if (writes > 0)
  {
    int w_index = index + 1;
    if (w_index >= num_pipes) {
      w_index -= num_pipes;
    }
    ::send(pipes[2 * w_index + 1], "a", 1, 0);
    writes--;
    fired++;
  }
  if (fired == count) {
    eventloop->Exit();
  }
}

struct timeval* RunOnce() {
  static struct timeval ta, ts, te;

  gettimeofday(&ta, NULL);

  for (int i = 0; i < num_pipes; ++i)
  {
    Dispatch& dispatch = dispatches[i];
    dispatch.SetReadCallback(std::bind(ReadCallback, dispatch.Fd(), i));
    dispatch.EnableRead();
  }

  int space = num_pipes / num_active;
  space *= 2;
  for (int i = 0; i < num_active; ++i)
  {
    ::send(pipes[i * space + 1], "a", 1, 0);
  }
  fired = num_active;
  count = 0;
  writes = num_writes;

  gettimeofday(&ts, NULL);
  eventloop->Loop();
  gettimeofday(&te, NULL);

  timersub(&te, &ta, &ta);
  timersub(&te, &ts, &ts);
  fprintf(stdout, "%8ld %8ld\n", 
          ta.tv_sec * 1000000L + ta.tv_usec, 
          ts.tv_sec * 1000000L + ts.tv_usec);

  return (&te);
}

int main(int argc, char* argv[]) {
  int i, c;
  struct timeval *tv;
  int *cp;
  extern char *optarg;

  num_pipes = 100;
  num_active = 1;
  num_writes = 100;

  while ((c = getopt(argc, argv, "n:a:w")) != -1) {
    switch (c) {
      case 'n':
        num_pipes = atoi(optarg);
        break;
      case 'a':
        num_active = atoi(optarg);
        break;
      case 'w':
        num_writes = atoi(optarg);
        break;
      default:
        fprintf(stderr, "Illegal argument \"%c\"\n", c);
        exit(1);
    }
  }
  
#if 1
  struct rlimit rl;
  rl.rlim_cur = rl.rlim_max = num_pipes * 2 + 50;
  if (::setrlimit(RLIMIT_NOFILE, &rl) == -1) {
    perror("setrlimit");
  }
#endif

  pipes = calloc(num_pipes * 2, sizeof(int));
  if (pipes == NULL)
  {
    perror("malloc");
    exit(1);
  }

  for (cp = pipes, i = 0; i < num_pipes; i++, cp += 2) {
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, cp)  == -1) {
      perror("pipe");
      exit(1);
    }
  }


  EventLoop ev;
  eventloop = &ev;
  int dispatch_size = num_pipes;
  dispatches(new scoped_ptr<Dispatch>[dispatch_size]);
  for (cp = pipes, i = 0; i < num_pipes; i++, cp += 2) {
    Dispatch* dispatch = new Dispatch(&ev, *cp);
    dispatches[i].reset(dispatch);
  }

  for (i = 0; i < 25; ++i) {
    tv = RunOnce();
  }

  for (i = 0; i < dispatch_size; ++i) {
    dispatches[i]->DisableAll();
    dispatches[i]->RemoveEvents();
  }
    
  return 0;
}
