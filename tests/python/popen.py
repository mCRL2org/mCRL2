#!/usr/bin/env python

# ~ Copyright 2014 Sjoerd Cranen, Wieger Wesselink
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import sys
import time
import psutil
import threading
import subprocess


class TimeExceededError(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)


class MemoryExceededError(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)


class Popen(subprocess.Popen):
    def __init__(self, *args, **kwargs):
        self.__maxVirtLimit = kwargs.setdefault('maxVirtLimit', 100000000)
        del kwargs['maxVirtLimit']
        self.__usrTimeLimit = kwargs.setdefault('usrTimeLimit', 100000000)
        del kwargs['usrTimeLimit']
        super(Popen, self).__init__(*args, **kwargs)
        self.__usrTime = 0
        self.__sysTime = 0
        self.__maxVirt = 0
        self.__maxResident = 0
        self.__perfThread = threading.Thread(target=self.__measure)
        self.__perfThread.daemon = True
        self.__perfThread.start()

    def __measure(self):
        try:
            process = psutil.Process(self.pid)
            while self.returncode is None:
                self.__usrTime, self.__sysTime = process.get_cpu_times()
                virt, res = process.get_memory_info()
                self.__maxVirt = max(self.__maxVirt, virt)
                self.__maxResident = max(self.__maxResident, res)
                if self.__maxVirt > self.__maxVirtLimit:
                    self.kill()
                    # raise MemoryExceededError(self.__maxVirt)
                if self.__usrTime > self.__usrTimeLimit:
                    self.kill()
                    # raise TimeExceededError(self.__usrTime)
                #with open("info.txt", "a") as myfile:
                #  myfile.write('usr = ' + str(self.__usrTime) + '\n')
                #  myfile.write('vms = ' + str(self.__maxVirt) + '\n')
                time.sleep(0.05)
        except psutil.NoSuchProcess:
            pass

    @property
    def userTime(self):
        return self.__usrTime

    @property
    def systemTime(self):
        return self.__sysTime

    @property
    def maxVirtualMem(self):
        return self.__maxVirt

    @property
    def maxResidentMem(self):
        return self.__maxResident

# This requires python3
#if __name__ == '__main__':
#  proc = Popen(sys.argv[1:])
#  ret = proc.wait()
#  print('usr/sys/virt/res: {0}/{1}/{2}/{3}'.format(proc.userTime, proc.systemTime, proc.maxVirtualMem, proc.maxResidentMem), file=sys.stderr)
#  sys.exit(ret)
