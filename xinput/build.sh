#!/bin/bash


gcc -o hook_xinput.so -fPIC -shared hook_xinput_api.c  -DHAVE_XI2  -lX11 -lXrandr -lXi -lXinerama
