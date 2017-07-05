import os

os.system('mcrl22lps -v parallel.mcrl2 parallel.lps')

os.system('lps2pbes -v -f nodeadlock.mcf parallel.lps parallel.nodeadlock.pbes')
os.system('pbes2bool -v parallel.nodeadlock.pbes')

# The following creates state space, and stores trace to deadlock.
os.system('lps2lts -v -Dt parallel.lps parallel.aut')

