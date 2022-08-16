#!/bin/bash

gcc -o main main.c -std=gnu99 -g -Wall `pkg-config x11 --cflags --libs`