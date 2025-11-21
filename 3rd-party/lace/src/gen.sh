#!/bin/bash
bash lace.sh 6 > lace.h
bash lace.sh 14 > lace14.h
sed "s:lace\.h:lace14\.h:g" lace.c > lace14.c
