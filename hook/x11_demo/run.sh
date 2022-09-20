#!/bin/bash



ulimit -c unlimited

LD_PRELOAD=/home/chensong/Work/cabroad_server/Server/ccloud_game_rtc/hook/hook_demo/prehook.so    ./build/x11_demo
