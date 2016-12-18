import os

os.system('mcrl22lps -v commprot.mcrl2 commprot.lps')

os.system('lps2pbes -v -f nodeadlock.mcf commprot.lps commprot.nodeadlock.pbes')
os.system('pbes2bool -v commprot.nodeadlock.pbes')


