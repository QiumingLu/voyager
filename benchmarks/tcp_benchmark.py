#!/usr/bin/python
from threading import Lock
import threading 
import socket
import time
import timeit
import sys

def add_request_count(count):
	global current_request
        lock_current_request.acquire()
        current_request += count
	if current_request % 1000 == 0:
		print "Completed: " + str(current_request) + "/" + str(total_request)
	lock_current_request.release()

def add_connect(time):
	global connect_sum
        lock_connect_sum.acquire()
        connect_sum += time
	lock_connect_sum.release()

def add_request(time):
	global request_sum
        lock_request_sum.acquire()
        request_sum += time
	lock_request_sum.release()

def add_connect_split(time):
	global connect_split
	lock_connect_split.acquire()
	times = 1
	if connect_split.has_key(time):
		times = connect_split[time]
	connect_split[time] = times + 1
	lock_connect_split.release()

def add_request_split(time):
	global request_split
	lock_request_split.acquire()
	times = 1
	if request_split.has_key(time):
		times = request_split[time]
	request_split[time] = times + 1
	lock_request_split.release()

def read_test_data(file):
	global test_data
	f = open(file)
	test_data = f.read().strip()

def socket_connect(send_count, is_show):
	s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)

	#start_connect = time.time()
	HOST='127.0.0.1'
	PORT=5666
	#HOST='10.170.0.226'
	#PORT=8080
	s.connect((HOST,PORT))
	#end_connect = time.time()

	#connect_cost = (end_connect - start_connect) * 1000
	#add_connect(connect_cost)
	#add_connect_split(int(connect_cost))
	
	count = 0
	while count < send_count:
		count = count + 1
		#start_request = time.time()
		#s.sendall("0\r\n\r\n113\r\n1405641431494,40.078094,116.327694,30|1405641421375,40.078218,116.327869,30|1405641426138,40.078111,116.327619,30\r\n")
		#s.sendall("POST http://10.1.152.72:5200 1.1\r\nContent-Type:text/xml\r\nContent-length:114\r\nConnection:keep-alive\r\n\r\n1405641431494,40.078094,116.327694,30|1405641421375,40.078218,116.327869,30|1405641426138,40.0781 11,116.327619,30");
		s.sendall("0\r\n\r\n" + str(len(test_data)) + "\r\n" + test_data + "\r\n")
		data = s.recv(1024)
		#if is_show:
		#	print data
		#end_request = time.time()
		#request_cost = (end_request - start_request) * 1000
		#add_request(request_cost)
		#add_request_split(int(request_cost))
		#add_request_count(1)
	s.close()

test_data = ""
test_file = sys.argv[3]

connect_sum = 0
connect_split = {}
request_sum = 0
request_split = {}

lock_connect_sum = Lock()
lock_request_sum = Lock()
lock_connect_split = Lock()
lock_request_split = Lock()
threads = []

total_concurrency = int(sys.argv[1])
total_request = int(sys.argv[2])
current_request = 0
lock_current_request = Lock()


read_test_data(test_file)
is_show = 0
if total_concurrency == 1 and total_request == 1:
	is_show = 1
	
for i in range(1, total_concurrency + 1):
	threads.append(threading.Thread(target=socket_connect, args=(total_request / total_concurrency, is_show,)))
for t in threads:
	t.start()

start_job = time.time()
for t in threads:
	t.join()
end_job = time.time()

total_time = end_job - start_job
print "Finished: " + str(current_request) + "/" + str(total_request)

print "\n======================================================="
print "Total time:           " + str(connect_sum + request_sum) + " [s]"
print "Total time:           " + str(total_time) + " [s] (across all concurrent requests)"
print "Time per request:     " + str((connect_sum + request_sum) / total_request * 1000) + " [ms] (mean)"
print "Time per request:     " + str(total_time / total_request * 1000) + " [ms] (mean, across all concurrent requests)"
print "Time per connect:     " + str(connect_sum / total_concurrency * 1000) + " [ms] (mean)"
print "Time per request:     " + str(request_sum / total_request * 1000) + " [ms] (mean, connecting time excluded)"
print "Requests per second:  " + str(int(total_request / total_time)) + " [#/sec] (mean)"

print "========================================================="

