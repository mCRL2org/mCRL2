#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-vD', 'house_santa_claus.mcrl2', 'house_santa_claus.lps'], check=True)

# Property draw_possible
# Is satisfied
print("\nProperty draw_possible")
subprocess.run(['lps2pbes', '-v', '-f', 'draw_possible.mcf',  'house_santa_claus.lps', 'house_santa_claus_draw_possible.pbes'], check=True)
subprocess.run(['pbessolve', '-v', 'house_santa_claus_draw_possible.pbes'], check=True)

# Property draw_from_roof
# Is violated
print("\nProperty draw_from_roof")
subprocess.run(['lps2pbes', '-v', '-f', 'draw_from_roof.mcf',  'house_santa_claus.lps', 'house_santa_claus_draw_from_roof.pbes'], check=True)
subprocess.run(['pbessolve', '-v', 'house_santa_claus_draw_from_roof.pbes'], check=True)
