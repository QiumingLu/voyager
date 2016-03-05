#!/bin/sh

killall server
timeout=${timeout:-100}
bufsize=16384
nothreads=1

for nosessions in 1 10 100 1000 10000; do
  sleep 5
  echo "Bufsize: $bufsize Threads: $nothreads Sessions: $nosessions"
  taskset -c 1 ../../build/debug/bin/server 0.0.0.0 55555 $nothreads & srvpid=$!
  taskset -c 2 ../../build/debug/bin/client 127.0.0.1 55555 $nothreads $bufsize $nosessions $timeout
  kill -9 $srvpid
done