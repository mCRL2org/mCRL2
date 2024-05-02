#!/usr/bin/env python3

import subprocess

subprocess.run(['mcrl22lps', '-v', '-o', 'cellular_automata.mcrl2', 'cellular_automata.lps'], shell=True, check=True)