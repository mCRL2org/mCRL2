#!/usr/bin/env python

import subprocess

i = 0
while True:
    try:
        name = subprocess.check_output(['python', 'test.py', '--name', str(i)])
        subprocess.Popen(['python', 'test.py', '--run', str(i)]).wait()
        i = i + 1
    except subprocess.CalledProcessError as e:
        break
