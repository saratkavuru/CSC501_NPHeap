##!/bin/bash
cd kernel_module
sudo rmmod npheap
make
sudo make install
sudo ismod npheap.ko
sudo chmod 777 /dev/npheap

