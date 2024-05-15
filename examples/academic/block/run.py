#!/usr/bin/env python3

import subprocess

subprocess.run(['mcrl22lps', 'block.mcrl2', 'block.lps'], check=True)

subprocess.run(['lps2pbes', '-f', 'nodeadlock.mcf', 'block.lps', 'block.nodeadlock.pbes'], check=True)
subprocess.run(['pbes2bool', 'block.nodeadlock.pbes'], check=True)
# The above shows that there is a deadlock in the specification, let's investigate.

# The following command creates the state space, and stores trace to the deadlock
subprocess.run(['lps2lts', '-Dt', 'block.lps', 'block.aut'], check=True)

print('Print the trace and find out what\'s wrong: ')
subprocess.run(['tracepp', 'block.lps_dlk_0.trc'], check=True)

# The same is also immediately clear if we visualise the state space, uncomment the following line.
# subprocess.run(['ltsgraph', 'allow.aut'], check=True)