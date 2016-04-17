#!/bin/sh

killall server
timeout=10
bufsize=16384

for nosessions in 100 1000; do
  for nothreads in 3 4; do
    sleep 5
    echo "Bufsize: $bufsize Threads: $nothreads Sessions: $nosessions"
    ../../build/release/bin/server 127.0.0.1 55555 $nothreads & srvpid=$!
    ../../build/release/bin/client 127.0.0.1 55555 $nothreads $bufsize $nosessions $timeout 
    kill -9 $srvpid
  done
done
