#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'dice.mcrl2', 'dice.lps'], check=True)
subprocess.run(['lps2lts', '-v', 'dice.lps', 'dice.aut'], check=True)
subprocess.run(['ltspbisim', '-epbisim', 'dice.aut', 'dice_reduced.aut'], check=True)

