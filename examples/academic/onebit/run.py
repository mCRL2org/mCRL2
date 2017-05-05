import os

os.system('mcrl22lps -v onebit.mcrl2 onebit.lps')
os.system('lps2pbes -v -f nodeadlock.mcf onebit.lps onebit.nodeadlock.pbes')
os.system('pbes2bool -v onebit.nodeadlock.pbes')

# Solving the following using pbes2bool is slow!
os.system('lps2pbes -v -f infinitely_often_enabled_then_infinitely_often_taken.mcf onebit.lps onebit.infinitely_often_enabled_then_infinitely_often_taken.pbes')
os.system('pbespgsolve -v -s2 onebit.infinitely_often_enabled_then_infinitely_often_taken.pbes')

os.system('lps2pbes -v -f infinitely_often_lost.mcf onebit.lps onebit.infinitely_often_lost.pbes')
os.system('pbespgsolve -v onebit.infinitely_often_lost.pbes')

os.system('lps2pbes -v -f infinitely_often_receive_dat1.mcf onebit.lps onebit.infinitely_often_receive_dat1.pbes')
os.system('pbes2bool -v onebit.infinitely_often_receive_dat1.pbes')

os.system('lps2pbes -v -f infinitely_often_receive_for_all_d.mcf onebit.lps onebit.infinitely_often_receive_for_all_d.pbes')
os.system('pbes2bool -v onebit.infinitely_often_receive_for_all_d.pbes')

os.system('lps2pbes -v -f read_then_eventually_send.mcf onebit.lps onebit.read_then_eventually_send.pbes')
os.system('pbes2bool -v onebit.read_then_eventually_send.pbes')

os.system('lps2pbes -v -f read_then_eventually_send_if_fair.mcf onebit.lps onebit.read_then_eventually_send_if_fair.pbes')
os.system('pbes2bool -v onebit.read_then_eventually_send_if_fair.pbes')

os.system('lps2pbes -v -f no_generation_of_messages.mcf onebit.lps onebit.no_generation_of_messages.pbes')
os.system('pbes2bool -v onebit.no_generation_of_messages.pbes')

os.system('lps2pbes -v -f no_duplication_of_messages.mcf onebit.lps onebit.no_duplication_of_messages.pbes')
os.system('pbes2bool -v onebit.no_duplication_of_messages.pbes')

