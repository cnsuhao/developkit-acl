#!/bin/sh

#valgrind --tool=memcheck --leak-check=yes -v ./redis_builder  -a create -f cluster.xml
#valgrind --tool=memcheck --leak-check=yes -v ./redis_builder  -a create -r 2 -f nodes2.xml
valgrind --tool=memcheck --leak-check=yes -v ./redis_builder  -a create -r 2 -f nodes3.xml
