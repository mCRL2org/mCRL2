#!/usr/bin/env', 'python3

import subprocess

subprocess.run(['mcrl22lps', '-v', 'allow.mcrl2', 'allow.lps'], shell=True, check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'nodeadlock.mcf', 'allow.lps', 'allow.nodeadlock.pbes'], shell=True, check=True)
subprocess.run(['pbes2bool', '-v', 'allow.nodeadlock.pbes'], shell=True, check=True)
# The above shows that there is a deadlock in the specification, let's investigate.

# The following command creates the state space, and stores trace to the deadlock
subprocess.run(['lps2lts', '-v', '-Dt', 'allow.lps', 'allow.aut'], shell=True, check=True)
print('Print the trace and find out what\'s wrong: ')
subprocess.run(['tracepp', 'allow.lps_dlk_0.trc'], shell=True, check=True)

# The same is also immediately clear if we visualise the state space, uncomment the following line.
# subprocess.run(['ltsgraph', 'allow.aut'], shell=True, check=True)