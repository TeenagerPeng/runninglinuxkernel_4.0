#!/bin/bash
# 如果是已经编译过内核就不用完全按照书里面的编译，直接用自己的编译产物，但是ARCH和CROSS_COMPILE需要指定对
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
make

