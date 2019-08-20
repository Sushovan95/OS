#!/bin/bash

gcc -fPIC -shared -o header.so header.c
gcc main.c ./header.so -lpthread