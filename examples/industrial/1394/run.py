import os

os.system('mcrl22lps -v 1394-fin.mcrl2 1394-fin.lps')

os.system('lps2pbes -v -f nodeadlock.mcf 1394-fin.lps 1394-fin.nodeadlock.pbes')
os.system('pbes2bool -v 1394-fin.nodeadlock.pbes')

os.system('lps2lts -v 1394-fin.lps 1394-fin.aut')

