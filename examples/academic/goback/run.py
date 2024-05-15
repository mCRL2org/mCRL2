#!/usr/bin/env python3

import subprocess

subprocess.run(['mcrl22lps', '-v', 'goback.mcrl2', 'goback.lps'], shell=True, check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'nodeadlock.mcf', 'goback.lps', 'goback.nodeadlock.pbes'], shell=True, check=True)
subprocess.run(['pbes2bool', '-v', 'goback.nodeadlock.pbes'], shell=True, check=True)