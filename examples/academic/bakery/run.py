import os

os.system('mcrl22lps -v bakery.mcrl2 bakery.lps')

os.system('lps2pbes -v -f nodeadlock.mcf bakery.lps bakery.nodeadlock.pbes')
os.system('pbes2bool -v bakery.nodeadlock.pbes')

os.system('lps2pbes -v -f request_can_eventually_enter.mcf bakery.lps bakery.request_can_eventually_enter.pbes')
os.system('pbes2bool -v bakery.request_can_eventually_enter.pbes')

os.system('lps2pbes -v -f request_must_eventually_enter.mcf bakery.lps bakery.request_must_eventually_enter.pbes')
os.system('pbes2bool -v bakery.request_must_eventually_enter.pbes')
