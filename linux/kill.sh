#!/bin/bash


ps -ef |grep rtc | grep -v grep | awk '{print $2}'| xargs kill -14
