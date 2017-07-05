import os

os.system('mcrl22lps -v garage-ver.mcrl2 garage-ver.lps')
os.system('lpssuminst -v -f garage-ver.lps | lpssumelm -v > garage-ver1.lps')

# The jittyc compiler is not available on windows. In that case use -rjitty.
os.system('lps2lts -v -rjittyc garage-ver1.lps')

os.system('lps2pbes -v -f nodeadlock.mcf garage-ver1.lps garage-ver.nodeadlock.pbes')
os.system('pbes2bool -v -rjittyc garage-ver.nodeadlock.pbes')


