import os

os.system('mcrl22lps -v clobber.mcrl2 clobber.lps')
os.system('lps2lts -vrjittyc clobber.lps clobber.aut')

os.system('lps2pbes -v -f nodeadlock.mcf clobber.lps clobber.nodeadlock.pbes')
os.system('pbes2bool -vrjittyc -s1 clobber.nodeadlock.pbes')

