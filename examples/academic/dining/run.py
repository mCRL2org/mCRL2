#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

for name in [
    'dining3',
    'dining3_cs',
    'dining3_cs_seq',
    'dining3_ns',
    'dining3_ns_seq',
    'dining3_schedule',
    'dining3_schedule_seq'
]:
    subprocess.run(['mcrl22lps', '-v', f'{name}.mcrl2', f'{name}.lps'], shell=True, check=True)

    subprocess.run(['lps2pbes', '-v', '-f', 'nodeadlock.mcf', f'{name}.lps', f'{name}.nodeadlock.pbes'], shell=True, check=True)
    subprocess.run(['pbes2bool', '-v', f'{name}.nodeadlock.pbes'], shell=True, check=True)
  
    subprocess.run(['lps2pbes', '-v', '-f', 'nostuffing.mcf', f'{name}.lps', f'{name}.nostuffing.pbes'], shell=True, check=True)
    subprocess.run(['pbes2bool', '-v', f'{name}.nostuffing.pbes'], shell=True, check=True)

    subprocess.run(['lps2pbes', '-v', '-f', 'nostarvation.mcf', f'{name}.lps', f'{name}.nostarvation.pbes'], shell=True, check=True)
    subprocess.run(['pbes2bool', '-v', f'{name}.nostarvation.pbes'], shell=True, check=True)
