import os

os.system('mcrl22lps -v shared_coin_protocol.mcrl2 shared_coin_protocol.lps')
os.system('lps2lts -v shared_coin_protocol.lps coin.aut')


