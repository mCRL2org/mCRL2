import os

os.system('mcrl22lps -vn lift3-final.mcrl2 lift3-final.lps')

os.system('lps2pbes -v -f nodeadlock.mcf lift3-final.lps lift3-final.nodeadlock.pbes')
os.system('pbes2bool -vrjittyc lift3-final.nodeadlock.pbes')

os.system('lps2lts -vrjittyc lift3-final.lps lift3-final.aut')

os.system('mcrl22lps -vn lift3-init.mcrl2 lift3-init.lps')

os.system('lps2pbes -v -f nodeadlock.mcf lift3-init.lps lift3-init.nodeadlock.pbes')
os.system('pbes2bool -vrjittyc lift3-init.nodeadlock.pbes')

os.system('lps2lts -vrjittyc lift3-init.lps lift3-init.aut')

os.system('ltscompare -vebisim lift3-init.aut lift3-final.aut')
os.system('lpsbisim2pbes -v -bstrong-bisim lift3-init.lps lift3-final.lps lift3-bisim.pbes')
os.system('pbes2bool -vrjittyc lift3-bisim.pbes')

