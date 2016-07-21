#include <utility>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#include "voyager/core/dispatch.h"
#include "voyager/core/eventloop.h"
#include "voyager/util/scoped_ptr.h"
#include "voyager/util/timestamp.h"

using namespace voyager;

EventLoop *eventloop;
scoped_array<scoped_ptr<Dispatch> > *g_dispatches;

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

std::pair<uint64_t, uint64_t> RunOnce() {

  Timestamp ta(Timestamp::Now());

  for (int i = 0; i < num_pipes; ++i)
  {
    (*g_dispatches)[i]->SetReadCallback(
        std::bind(ReadCallback, (*g_dispatches)[i]->Fd(), i));
    (*g_dispatches)[i]->EnableRead();
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

  Timestamp ts(Timestamp::Now());
  eventloop->Loop();
  Timestamp te(Timestamp::Now());

  uint64_t total_time = te.MicroSecondsSinceEpoch() - 
                            ta.MicroSecondsSinceEpoch();
  uint64_t sub_time = te.MicroSecondsSinceEpoch() - ts.MicroSecondsSinceEpoch();
  std::pair<uint64_t, uint64_t> t = std::make_pair(total_time, sub_time);

  fprintf(stdout, "%8" PRId64 "%8" PRId64 "\n", total_time, sub_time);

  return t;
}

int main(int argc, char* argv[]) {
  int i, c;
  int* cp;
  extern char *optarg;

  num_pipes = 100;
  num_active = 1;
  num_writes = 100;

  while ((c = getopt(argc, argv, "n:a:w:")) != -1) {
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
  
  printf("num_pipes:%d\n", num_pipes);
  printf("num_active:%d\n", num_active);
  printf("num_writes:%d\n", num_writes);

#if 1
  struct rlimit rl;
  rl.rlim_cur = rl.rlim_max = num_pipes * 2 + 50;
  if (::setrlimit(RLIMIT_NOFILE, &rl) == -1) {
    perror("setrlimit");
  }
#endif

  pipes = static_cast<int*>(calloc(num_pipes * 2, sizeof(int)));
  if (pipes == NULL)
  {
    perror("malloc");
    exit(1);
  }

  for (cp = pipes, i = 0; i < num_pipes; i++, cp += 2) {
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, cp)  == -1) {
      perror("pipe");
      free(pipes);
      exit(1);
    }
  }


  EventLoop ev;
  eventloop = &ev;
  int dispatch_size = num_pipes;

  scoped_array<scoped_ptr<Dispatch> > dispatches(
      new scoped_ptr<Dispatch>[dispatch_size]);
  for (cp = pipes, i = 0; i < num_pipes; i++, cp += 2) {
    Dispatch* dispatch = new Dispatch(&ev, *cp);
    dispatches[i].reset(dispatch);
  }

  g_dispatches = &dispatches;

  uint64_t total_times = 0;
  uint64_t sub_times = 0;
  for (i = 0; i < 25; ++i) {
    std::pair<uint64_t, uint64_t> t = RunOnce();
    if (i != 0) {
      total_times += t.first;
      sub_times += t.second;
    }
  }

  fprintf(stdout, "TotolTime: %8" PRId64 ", SubTime: %8" PRId64 "\n", total_times/24, sub_times/24);

  for (i = 0; i < dispatch_size; ++i) {
    dispatches[i]->DisableAll();
    dispatches[i]->RemoveEvents();
  }

  free(pipes);
    
  return 0;
}
