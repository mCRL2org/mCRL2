#!/usr/bin/env python3

import subprocess

subprocess.run(['mcrl22lps', '-v', 'producer_consumer.mcrl2', 'producer_consumer.lps'],  check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'nodeadlock.mcf', 'producer_consumer.lps', 'producer_consumer.nodeadlock.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'producer_consumer.nodeadlock.pbes'],  check=True)

# The following creates state space, and stores trace to deadlock.
subprocess.run(['lps2lts', '-v', '-Dt', 'producer_consumer.lps', 'producer_consumer.aut'],  check=True)

# However, the initial state is a deadlock so the trace is empty.
subprocess.run(['tracepp', 'producer_consumer.lps_dlk_0.trc'], check=True)
