import os
# The generated state space is large. This run is time consuming. 
os.system('mcrl22lps -v commprot.mcrl2 commprot.lps')
os.system('lps2pbes -v -f -nodeadlock.mcf commprot.lps commprot.nodeadlock.pbes')
os.system('pbes2bool -v -s2 -rjittyc commprot.nodeadlock.pbes')


