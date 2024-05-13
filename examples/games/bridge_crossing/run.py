#!/usr/bin/env python3

import subprocess

subprocess.run(['mcrl22lps', '-v', 'bridge_crossing.mcrl2', 'bridge_crossing.lps'], check=True)
subprocess.run(['lps2lts', '-v', '-a', 'ready', 'bridge_crossing.lps', '-t5'], check=True)
subprocess.run(['tracepp', 'bridge_crossing.lps_act_2_ready.trc'], check=True)
