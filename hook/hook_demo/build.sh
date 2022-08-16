#!/bin/bash

gcc -o main main.c -g -Wall

# -lglut -lGLU
gcc -o prehook.so -fPIC -shared prehook.c  -I/usr/local/include  -L/usr/local/lib -L/usr/X11R6/lib -ldl   -lGL

