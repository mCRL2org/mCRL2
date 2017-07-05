import os

os.system('mcrl22lps -v producer_consumer.mcrl2 producer_consumer.lps')

os.system('lps2pbes -v -f nodeadlock.mcf producer_consumer.lps producer_consumer.nodeadlock.pbes')
os.system('pbes2bool -v producer_consumer.nodeadlock.pbes')

# The following creates state space, and stores trace to deadlock.
os.system('lps2lts -v -Dt producer_consumer.lps producer_consumer.aut')

# Print trace to deadlock
os.system('tracepp producer_consumer.lps_dlk_0.trc')
# The initial state is a deadlock, so the trace is empty

