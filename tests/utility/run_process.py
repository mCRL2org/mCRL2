#!/usr/bin/env python

# ~ Copyright 2014-2025 Sjoerd Cranen, Wieger Wesselink, Maurice Laveaux
# ~ Distributed under the Boost Software License, Version 1.0.
# ~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import time
import psutil
import subprocess
import platform
import concurrent.futures
import signal

class TimeExceededError(Exception):
    def __init__(self, name: str, value: float, max_time: float):
        self.name = name
        self.value = value
        self.max_time = max_time

    def __str__(self):
        return f"Tool {self.name} exceeded time after {self.value:.2f}s of max {self.max_time:.2f}s"


class MemoryExceededError(Exception):
    def __init__(self, name: str, value: float, max_memory: float):
        self.name = name
        self.value = value
        self.max_memory = max_memory

    def __str__(self):
        return f"Tool {self.name} exceeded memory with {self.value:.2f}MB of max {self.max_memory:.2f}MB"


class StackOverflowError(Exception):
    def __init__(self, name: str):
        self.name = name

    def __str__(self):
        return f"Stack overflow in tool {self.name}"


class SegmentationFault(Exception):
    def __init__(self, name: str):
        self.name = name

    def __str__(self):
        return f"Segmentation fault in tool {self.name}"


class ToolNotFoundError(Exception):
    def __init__(self, name: str):
        self.name = name

    def __str__(self):
        return f"Tool {self.name} does not exist!"


class ToolRuntimeError(Exception):
    def __init__(self, value: str):
        self.value = value

    def __str__(self):
        return repr(self.value)

def kill_all(process, sig=signal.SIGTERM):
    """Kill a process tree (including grandchildren) with signal
    "sig" and return a (gone, still_alive) tuple.
    """
    children = process.children(recursive=True)
    children.append(process)

    for p in children:
        try:
            p.kill()
        except psutil.NoSuchProcess:
            pass
    _, alive = psutil.wait_procs(children)
    assert not alive

class RunProcess:
    stdout = ""
    stderr = ""
    returncode = -1

    def __init__(
        self, tool: str, arguments: list[str], max_memory: float, max_time: float
    ):
        """
        Run the process tool with the given arguments, using at most max_memory MB of memory, and max_time seconds
        """

        if platform.system() == "Windows":
            # Don't display the Windows GPF dialog if the invoked program dies.
            # See comp.os.ms-windows.programmer.win32
            # How to suppress crash notification dialog?, Raymond Chen Jan 14,2004 -
            import ctypes

            SEM_NOGPFAULTERRORBOX = 0x0002  # From MSDN
            ctypes.windll.kernel32.SetErrorMode(SEM_NOGPFAULTERRORBOX)
            subprocess_flags = 0x8000000  # win32con.CREATE_NO_WINDOW?

            if not tool.lower().endswith(".exe"):
                tool += ".exe"
        else:
            subprocess_flags = 0

        try:
            with subprocess.Popen(
                [tool] + arguments,
                stdout=subprocess.PIPE,
                stdin=subprocess.PIPE,
                stderr=subprocess.PIPE,
                creationflags=subprocess_flags,
            ) as proc:
                self._user_time = 0
                self._max_memory_used = 0

                # Start a thread to limit the process memory and time usage.
                def enforce_limits(proc):
                    try:
                        process = psutil.Process(proc.pid)
                        while proc.returncode is None:
                            m = process.memory_info()

                            self._max_memory_used = max(
                                self._max_memory_used, m.rss / 1024 / 1024
                            )

                            if self._max_memory_used > max_memory:
                                kill_all(process)
                                raise MemoryExceededError(
                                    tool, self._max_memory_used, max_memory
                                )
                            if self._user_time > max_time:
                                kill_all(process)
                                raise TimeExceededError(tool, self._user_time, max_time)
                            self._user_time += 0.1
                            time.sleep(0.1)

                    except psutil.NoSuchProcess as _:
                        # The tool finished before we could acquire the pid
                        pass

                with concurrent.futures.ThreadPoolExecutor(max_workers=1) as executor:
                    future = executor.submit(enforce_limits, proc)

                    stdout, stderr = proc.communicate()
                    self.stdout = stdout.decode("utf-8", errors="replace")
                    self.stderr = stderr.decode("utf-8", errors="replace")
                    self.returncode = proc.returncode
                    future.result()

                if proc.returncode != 0:
                    print(self.stderr)
                    raise ToolRuntimeError(
                        f"Tool {tool} {arguments} ended with return code {proc.returncode}"
                    )
                if platform.system() == "Windows" and proc.returncode == -1073740777:
                    raise ToolRuntimeError(
                        f"Tool {tool} {arguments} failed with the return code STATUS_INVALID_CRUNTIME_PARAMETER (0xC0000417)"
                    )
                if platform.system() == "Windows" and proc.returncode == -1073741571:
                    raise StackOverflowError(tool)
                if platform.system() == "Linux" and proc.returncode == -11:
                    raise SegmentationFault(tool)
        except FileNotFoundError as e:
            raise ToolNotFoundError(tool) from e

    @property
    def user_time(self):
        return self._user_time

    @property
    def max_memory(self):
        return self._max_memory_used
