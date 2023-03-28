#!/bin/bash

#gcc -o main main.c -g -Wall  -I/usr/local/include  -L/usr/local/lib -L/usr/X11R6/lib -ldl   -lGL -lX11


#gcc -o cb_send_event_toutch cb_send_event_toutch.c -g -Wall  -I/usr/local/include  -L/usr/local/lib -L/usr/X11R6/lib -ldl   -lGL -lX11 -lxcb


# -lglut -lGLU

gcc -o prehook.so -fPIC -shared prehook.c  -I/usr/local/include  -L/usr/local/lib -L/usr/X11R6/lib -ldl   -lGL

