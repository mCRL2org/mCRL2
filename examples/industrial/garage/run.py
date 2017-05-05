import os

os.system('mcrl22lps -v garage-ver.mcrl2 garage-ver.lps')

os.system('lps2pbes -v -f nodeadlock.mcf garage-ver.lps garage-ver.nodeadlock.pbes')
# The jittyc compiler is not available on windows. In that case use -rjitty.
os.system('pbes2bool -v -rjittyc garage-ver.nodeadlock.pbes')

os.system('lps2lts -v -rjittyc --cached garage-ver.lps garage-ver.aut')

