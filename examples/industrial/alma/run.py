import os

os.system('mcrl22lps -v alma.mcrl2 alma.lps')

os.system('lps2pbes -v -f nodeadlock.mcf alma.lps alma.nodeadlock.pbes')
os.system('pbes2bool -v alma.nodeadlock.pbes')

os.system('lps2lts -v alma.lps alma.aut')

