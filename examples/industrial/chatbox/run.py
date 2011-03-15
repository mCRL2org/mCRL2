import os

os.system('mcrl22lps -v chatbox.mcrl2 chatbox.lps')

os.system('lps2pbes -v -f nodeadlock.mcf chatbox.lps chatbox.nodeadlock.pbes')
os.system('pbes2bool -v chatbox.nodeadlock.pbes')

os.system('lps2lts -v chatbox.lps chatbox.aut')

