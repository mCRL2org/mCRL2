#!/usr/bin/env python

# ~ Copyright 2014 Sjoerd Cranen, Wieger Wesselink
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

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


class StackOverflowError(Exception):
    def __init__(self, name):
        self.name = name

    def __str__(self):
        return 'Stack overflow in tool {}'.format(self.name)


class SegmentationFault(Exception):
    def __init__(self, name):
        self.name = name

    def __str__(self):
        return 'Segmentation fault in tool {}'.format(self.name)


class ToolNotFoundError(Exception):
    def __init__(self, name):
        self.name = name

    def __str__(self):
        return 'Tool {} does not exist!'.format(self.name)


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

        # workaround for interface changes in psutil
        process = psutil.Process(self.pid)
	if "get_cpu_times" in dir(process):
            self.__perfThread = threading.Thread(target=self.__measure_old)
        else:
            self.__perfThread = threading.Thread(target=self.__measure_new)

        self.__perfThread.daemon = True
        self.__perfThread.start()

    # uses old interface of psutil
    def __measure_old(self):
        try:
            process = psutil.Process(self.pid)
            while self.returncode is None:
                self.__usrTime, self.__sysTime = process.cpu_times()
                virt, res = process.memory_info()
                self.__maxVirt = max(self.__maxVirt, virt)
                self.__maxResident = max(self.__maxResident, res)
                if self.__maxVirt > self.__maxVirtLimit:
                    self.kill()
                    # raise MemoryExceededError(self.__maxVirt)
                if self.__usrTime > self.__usrTimeLimit:
                    self.kill()
                    # raise TimeExceededError(self.__usrTime)
                time.sleep(0.05)
        except psutil.NoSuchProcess:
            pass

    # uses new interface of psutil
    def __measure_new(self):
        try:
            process = psutil.Process(self.pid)
            while self.returncode is None:
                t = process.cpu_times()
                m = process.memory_info()
                self.__usrTime, self.__sysTime = t.user, t.system
                self.__maxVirt = max(self.__maxVirt, m.vms)
                self.__maxResident = max(self.__maxResident, m.rss)
                if self.__maxVirt > self.__maxVirtLimit:
                    self.kill()
                    # raise MemoryExceededError(self.__maxVirt)
                if self.__usrTime > self.__usrTimeLimit:
                    self.kill()
                    # raise TimeExceededError(self.__usrTime)
                time.sleep(0.05)
        except psutil.NoSuchProcess:
            pass

    @property
    def user_time(self):
        return self.__usrTime

    @property
    def system_time(self):
        return self.__sysTime

    @property
    def max_virtual_memory(self):
        return self.__maxVirt

    @property
    def max_resident_memory(self):
        return self.__maxResident

# This requires python3
#if __name__ == '__main__':
#  proc = Popen(sys.argv[1:])
#  ret = proc.wait()
#  print('usr/sys/virt/res: {0}/{1}/{2}/{3}'.format(proc.user_time, proc.system_time, proc.max_virtual_memory, proc.max_resident_memory), file=sys.stderr)
#  sys.exit(ret)
