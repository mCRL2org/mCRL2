import os

os.system('mcrl22lps -v cabp.mcrl2 cabp.lps')
os.system('lps2pbes -v -f nodeadlock.mcf cabp.lps cabp.nodeadlock.pbes')
os.system('pbes2bool -v cabp.nodeadlock.pbes')

os.system('lps2pbes -v -f infinitely_often_enabled_then_infinitely_often_taken.mcf cabp.lps cabp.infinitely_often_enabled_then_infinitely_often_taken.pbes')
os.system('pbespgsolve -v cabp.infinitely_often_enabled_then_infinitely_often_taken.pbes')

os.system('lps2pbes -v -f infinitely_often_lost.mcf cabp.lps cabp.infinitely_often_lost.pbes')
os.system('pbes2bool -v cabp.infinitely_often_lost.pbes')

os.system('lps2pbes -v -f infinitely_often_receive_d1.mcf cabp.lps cabp.infinitely_often_receive_d1.pbes')
os.system('pbespgsolve -v cabp.infinitely_often_receive_d1.pbes')

os.system('lps2pbes -v -f infinitely_often_receive_for_all_d.mcf cabp.lps cabp.infinitely_often_receive_for_all_d.pbes')
os.system('pbespgsolve -v cabp.infinitely_often_receive_for_all_d.pbes')

os.system('lps2pbes -v -f read_then_eventually_send.mcf cabp.lps cabp.read_then_eventually_send.pbes')
os.system('pbes2bool -v cabp.read_then_eventually_send.pbes')

os.system('lps2pbes -v -f read_then_eventually_send_if_fair.mcf cabp.lps cabp.read_then_eventually_send_if_fair.pbes')
os.system('pbes2bool -v cabp.read_then_eventually_send_if_fair.pbes')

os.system('lps2pbes -v -f no_generation_of_messages.mcf cabp.lps cabp.no_generation_of_messages.pbes')
os.system('pbes2bool -v cabp.no_generation_of_messages.pbes')

os.system('lps2pbes -v -f no_duplication_of_messages.mcf cabp.lps cabp.no_duplication_of_messages.pbes')
os.system('pbes2bool -v cabp.no_duplication_of_messages.pbes')

