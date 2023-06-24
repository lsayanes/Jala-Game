
#! /bin/bash
echo "Valgrind test $1"
valgrind --track-fds=yes --leak-check=full --show-leak-kinds=all --log-file=./valgrind.log $1 -ddf
