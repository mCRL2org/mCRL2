import os

os.system('mcrl22lps -v par.mcrl2 par.lps')

os.system('lps2pbes -v -f nodeadlock.mcf par.lps par.nodeadlock.pbes')
os.system('pbes2bool -v par.nodeadlock.pbes')

# The following creates state space, and stores trace to deadlock.
os.system('lps2lts -v -Dt par.lps par.aut')

