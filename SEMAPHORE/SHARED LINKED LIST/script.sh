#!/bin/bash

gcc -fPIC -shared -Wall -o llist.so llist.c
gcc userprog.c ./llist.so
