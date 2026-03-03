#!/bin/bash
bash lace.sh 10 64 > lace.h
bash lace.sh 14 128 > lace14.h
sed "s:lace\.h:lace14\.h:g" lace.c > lace14.c
