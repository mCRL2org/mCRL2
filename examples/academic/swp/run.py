import os

# SWP_lists
os.system('mcrl22lps -v swp_lists.mcrl2 swp_lists.lps')
os.system('lps2pbes -v -f nodeadlock.mcf swp_lists.lps swp_lists.nodeadlock.pbes')
os.system('pbes2bool -v swp_lists.nodeadlock.pbes')

os.system('lps2pbes -v -f infinitely_often_enabled_then_infinitely_often_taken.mcf swp_lists.lps swp_lists.infinitely_often_enabled_then_infinitely_often_taken.pbes')
os.system('pbes2pgsolve -srecursive -v swp_lists.infinitely_often_enabled_then_infinitely_often_taken.pbes')

os.system('lps2pbes -v -f infinitely_often_lost.mcf swp_lists.lps swp_lists.infinitely_often_lost.pbes')
os.system('pbespgsolve -srecursive -v swp_lists.infinitely_often_lost.pbes')

os.system('lps2pbes -v -f infinitely_often_receive_d1.mcf swp_lists.lps swp_lists.infinitely_often_receive_d1.pbes')
os.system('pbespgsolve -srecursive -v swp_lists.infinitely_often_receive_d1.pbes')

os.system('lps2pbes -v -f infinitely_often_receive_for_all_d.mcf swp_lists.lps swp_lists.infinitely_often_receive_for_all_d.pbes')
os.system('pbespgsolve -srecursive -v swp_lists.infinitely_often_receive_for_all_d.pbes')

os.system('lps2pbes -v -f read_then_eventually_send.mcf swp_lists.lps swp_lists.read_then_eventually_send.pbes')
os.system('pbespgsolve -srecursive -v swp_lists.read_then_eventually_send.pbes')

os.system('lps2pbes -v -f read_then_eventually_send_if_fair.mcf swp_lists.lps swp_lists.read_then_eventually_send_if_fair.pbes')
os.system('pbespgsolve -srecursive -v swp_lists.read_then_eventually_send_if_fair.pbes')

os.system('lps2pbes -v -f no_generation_of_messages.mcf swp_lists.lps swp_lists.no_generation_of_messages.pbes')
os.system('pbespgsolve -srecursive -v swp_lists.no_generation_of_messages.pbes')

os.system('lps2pbes -v -f no_duplication_of_messages.mcf swp_lists.lps swp_lists.no_duplication_of_messages.pbes')
os.system('pbes2bool -srecursive -v swp_lists.no_duplication_of_messages.pbes')

# SWP with Tanenbaum's bug
os.system('mcrl22lps -v swp_with_tanenbaums_bug.mcrl2 swp_with_tanenbaums_bug.lps')
os.system('lps2pbes -v -f nodeadlock.mcf swp_with_tanenbaums_bug.lps swp_with_tanenbaums_bug.nodeadlock.pbes')
os.system('pbes2bool -v swp_with_tanenbaums_bug.nodeadlock.pbes')

# Generate state space, and look for occurrence of error action. Store trace,
# and stop generating after finding a single trace
os.system('lps2lts -v -aerror -t1 -rjittyc swp_with_tanenbaums_bug.lps swp_with_tanenbaums_bug.aut')
