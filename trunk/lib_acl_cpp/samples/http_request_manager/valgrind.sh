#!/bin/sh

#valgrind --tool=memcheck --leak-check=yes -v ././http_request_manager -s "www.qq.com:81"
valgrind --tool=memcheck --leak-check=yes -v ./http_request_manager -s "202.108.33.60:80;211.150.74.6:80;61.135.167.36:81"
