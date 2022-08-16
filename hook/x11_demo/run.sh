#!/bin/bash



ulimit -c unlimited

LD_PRELOAD=/home/chensong/Work/game/hook_demo/prehook.so    ./build/x11_demo
