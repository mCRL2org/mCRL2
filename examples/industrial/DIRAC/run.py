#!/usr/bin/env python3

import subprocess
import os

from shutil import which

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-n', 'WMS.mcrl2', 'WMS.lps'], check=True)

lpsreach = which('lpsreach')
if lpsreach is not None:
    subprocess.run([lpsreach, '--chaining', '--groups=simple', '--saturation'], check=True)