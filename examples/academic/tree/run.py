import os

os.system('mcrl22lps -v tree.mcrl2 tree.lps')

os.system('lps2pbes -v -f nodeadlock.mcf tree.lps tree.nodeadlock.pbes')
os.system('pbes2bool -v tree.nodeadlock.pbes')
# The above shows that there is a deadlock in the specification.
# Let's investigate

os.system('lps2lts -v tree.lps tree.aut')
# The following creates state space, and stores traces to 512 deadlocks.
#os.system('lps2lts -v -Dt tree.lps tree.aut')
# Print the trace and find out what's wrong:
#os.system('tracepp tree.lps_dlk_0.trc')

# The state space in ltsview resembles modern art...
os.system('ltsview tree.aut')


