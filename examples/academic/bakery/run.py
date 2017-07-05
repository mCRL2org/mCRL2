import os
# Not all of the below terminate!
os.system('mcrl22lps -v bakery.mcrl2 bakery.lps')

os.system('lps2pbes -v -f nodeadlock.mcf bakery.lps bakery.nodeadlock.pbes')
# os.system('pbes2bool -v -s2 -rjitty bakery.nodeadlock.pbes')  Does not terminate.

os.system('lps2pbes -v -f request_can_eventually_enter.mcf bakery.lps bakery.request_can_eventually_enter.pbes')
# os.system('pbes2bool -v -s3 -rjitty bakery.request_can_eventually_enter.pbes') Does not terminate.

os.system('lps2pbes -v -f request_must_eventually_enter.mcf bakery.lps bakery.request_must_eventually_enter.pbes')
# os.system('pbes2bool -v -s3 -rjitty bakery.request_must_eventually_enter.pbes') Does not terminate.

os.system('lps2pbes -v -f mutual_exclusion.mcf bakery.lps bakery.mutual_exclusion.pbes')
# os.system('pbes2bool -v -s3 -rjitty bakery.mutual_exclusion.pbes') Does not terminate.

os.system('lps2pbes -v -f always_can_get_number.mcf bakery.lps bakery.always_can_get_number.pbes')
#os.system('pbes2bool -v -s3 -rjitty bakery.always_can_get_number.pbes') Does not terminate.

os.system('lps2pbes -v -f get_at_least_number_circulating.mcf bakery.lps bakery.get_at_least_number_circulating.pbes')
# os.system('pbes2bool -v -s3 -rjitty bakery.get_at_least_number_circulating.pbes')  Does not terminate.
