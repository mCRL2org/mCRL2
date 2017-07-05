import os

os.system('mcrl22lps -v brp.mcrl2 brp.lps')

os.system('lps2pbes -v -f nodeadlock.mcf brp.lps brp.nodeadlock.pbes')
os.system('pbes2bool -v brp.nodeadlock.pbes')

os.system('lps2lts -v brp.lps brp.aut')

