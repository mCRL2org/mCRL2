#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'SMMT_Semantics_Model_Figure_1.mcrl2', 'SMMT_Semantics_Model_Figure_1.lps'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'nodeadlock.mcf', 'SMMT_Semantics_Model_Figure_1.lps', 'SMMT_Semantics_Model_Figure_1.nodeadlock.pbes'], check=True)
subprocess.run(['pbessolve', '-v', 'SMMT_Semantics_Model_Figure_1.nodeadlock.pbes'], check=True)

subprocess.run(['lps2lts', '-v', 'SMMT_Semantics_Model_Figure_1.lps', 'SMMT_Semantics_Model_Figure_1.aut'], check=True)

