import os

os.system('mcrl22lps -v goback.mcrl2 goback.lps')

os.system('lps2pbes -v -f nodeadlock.mcf goback.lps goback.nodeadlock.pbes')
os.system('pbes2bool -v goback.nodeadlock.pbes')


