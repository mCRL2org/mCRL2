import os

os.system('mcrl22lps -v garage-ver.mcrl2 garage-ver.lps')

os.system('lps2pbes -v -f nodeadlock.mcf garage-ver.lps garage-ver.nodeadlock.pbes')
os.system('pbes2bool -v garage-ver.nodeadlock.pbes')

os.system('lps2lts -v garage-ver.lps garage-ver.aut')

