import os

os.system('mcrl22lps -vD bke.mcrl2 bke.lps')
# Running any transformation does not have any effect, due to the following
# error:
# Error while exploring state space: error: term does not evaluate to true or false: address(E) == bad_address
# (JK: Checked 14/9/2010)
#os.system('lps2pbes -v -f nodeadlock.mcf bke.lps bke.nodeadlock.pbes')
#os.system('pbes2bool -v bke.nodeadlock.pbes')


