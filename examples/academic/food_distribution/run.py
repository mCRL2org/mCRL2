import os

os.system('mcrl22lps -v food_package.mcrl2 food_package.lps')
os.system('lpssuminst food_package.lps food_package.lpssuminst.lps')
os.system('lps2pbes -v -f sustained_delivery.mcf food_package.lpssuminst.lps sustained_delivery.pbes')
os.system('pbesconstelm -ve sustained_delivery.pbes sustained_delivery.pbesconstelm.pbes')
# If possible use -rjittyc below, which does work on linux and mac, and not on windows, as
# the generated bes is huge.
os.system('pbes2bool -v sustained_delivery.pbesconstelm.pbes')


