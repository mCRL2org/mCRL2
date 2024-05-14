#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'hopcroft.mcrl2', 'hopcroft.lps'], shell=True, check=True)
subprocess.run(['lps2lts', '-v', 'hopcroft.lps', '--no-info', 'hopcroft.lts'], shell=True, check=True)
subprocess.run(['ltsconvert', '-ebisim', 'hopcroft.lts', '-v', 'hopcroft.bisim.lts'], shell=True, check=True)
