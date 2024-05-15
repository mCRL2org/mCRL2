#!/usr/bin/env python3

import subprocess

subprocess.run(['mcrl22lps', '-v', 'mpsu.mcrl2', 'mpsu.lps'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'nodeadlock.mcf', 'mpsu.lps', 'mpsu.nodeadlock.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'mpsu.nodeadlock.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'mpsu1.mcf', 'mpsu.lps', 'mpsu.mpsu1.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'mpsu.mpsu1.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'mpsu2.mcf', 'mpsu.lps', 'mpsu.mpsu2.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'mpsu.mpsu2.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'mpsu3.mcf', 'mpsu.lps', 'mpsu.mpsu3.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'mpsu.mpsu3.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'mpsu4.mcf', 'mpsu.lps', 'mpsu.mpsu4.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'mpsu.mpsu4.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'mpsu5.mcf', 'mpsu.lps', 'mpsu.mpsu5.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'mpsu.mpsu5.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'mpsu6.mcf', 'mpsu.lps', 'mpsu.mpsu6.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'mpsu.mpsu6.pbes'], check=True)
