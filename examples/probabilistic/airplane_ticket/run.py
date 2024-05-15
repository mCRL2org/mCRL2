#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'airplane_ticket.mcrl2', 'airplane_ticket.lps'], check=True)
subprocess.run(['lps2lts', '-v', 'airplane_ticket.lps', 'airplane_ticket.aut'], check=True)
