#!/usr/bin/env python3

import subprocess
import shutil
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'swp_lists.mcrl2', 'swp_lists.lps'], check=True)
subprocess.run(['lps2pbes', '-v', '-f', 'nodeadlock.mcf', 'swp_lists.lps', 'swp_lists.nodeadlock.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'swp_lists.nodeadlock.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'infinitely_often_enabled_then_infinitely_often_taken.mcf', 'swp_lists.lps', 'swp_lists.infinitely_often_enabled_then_infinitely_often_taken.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'swp_lists.infinitely_often_enabled_then_infinitely_often_taken.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'infinitely_often_lost.mcf', 'swp_lists.lps', 'swp_lists.infinitely_often_lost.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'swp_lists.infinitely_often_lost.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'infinitely_often_receive_d1.mcf', 'swp_lists.lps', 'swp_lists.infinitely_often_receive_d1.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'swp_lists.infinitely_often_receive_d1.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'infinitely_often_receive_for_all_d.mcf', 'swp_lists.lps', 'swp_lists.infinitely_often_receive_for_all_d.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'swp_lists.infinitely_often_receive_for_all_d.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'read_then_eventually_send.mcf', 'swp_lists.lps', 'swp_lists.read_then_eventually_send.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'swp_lists.read_then_eventually_send.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'read_then_eventually_send_if_fair.mcf', 'swp_lists.lps', 'swp_lists.read_then_eventually_send_if_fair.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'swp_lists.read_then_eventually_send_if_fair.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'no_generation_of_messages.mcf', 'swp_lists.lps', 'swp_lists.no_generation_of_messages.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'swp_lists.no_generation_of_messages.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'no_duplication_of_messages.mcf', 'swp_lists.lps', 'swp_lists.no_duplication_of_messages.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'swp_lists.no_duplication_of_messages.pbes'], check=True)

# SWP with Tanenbaum's bug
subprocess.run(['mcrl22lps', '-v', 'swp_with_tanenbaums_bug.mcrl2', 'swp_with_tanenbaums_bug.lps'], check=True)
subprocess.run(['lps2pbes', '-v', '-f', 'nodeadlock.mcf', 'swp_with_tanenbaums_bug.lps', 'swp_with_tanenbaums_bug.nodeadlock.pbes'], check=True)

# Verifying deadlockfreedom with two data elements and two buffer positions
# leads to a state space that is too large for the explicit tools. However, the
# symbolic tools (which are only available on linux and macos) are able to at
# least show the state space sizes.
lpsreach = shutil.which('lpsreach')
if lpsreach and '--long' in argv:
    # number of states = 1.87117e+10 (time = 3762.31s)
    subprocess.run([lpsreach, '-v', '--cached', '--chaining', '--saturation', 'swp_with_tanenbaums_bug.lps'], check=True)

pbessolvesymbolic = shutil.which('pbessolvesymbolic')
if pbessolvesymbolic and '--long' in argv:
    # number of BES equations = 5.61351e+10 (time = 5654.58s)
    # finished solving (time = 13.56s)
    # true
    subprocess.run([pbessolvesymbolic, '-v', '-c', '--cached', '--chaining', '--saturation', 'swp_with_tanenbaums_bug.nodeadlock.pbes'], check=True)

# It is possible to generate the state space, while looking for occurrence of
# error action, because the error action can be found without exploring the
# whole state space. The command below stores trace, and stops generating after
# finding a single trace. Removal of --cached leads to less memory requirements,
# at the expense of a longer running time. 
subprocess.run(['lps2lts', '-v', '--cached', '-aerror', '-t1', 'swp_with_tanenbaums_bug.lps'], check=True)
