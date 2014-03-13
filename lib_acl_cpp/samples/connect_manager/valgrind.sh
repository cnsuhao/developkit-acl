#!/bin/sh

valgrind --tool=memcheck --leak-check=yes -v ././connect_manager -s "www.qq.com:81"
