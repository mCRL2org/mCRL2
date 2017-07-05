import os

os.system('mcrl22lps -v block.mcrl2 block.lps')

os.system('lps2pbes -v -f nodeadlock.mcf block.lps block.nodeadlock.pbes')
os.system('pbes2bool -v block.nodeadlock.pbes')
# The above shows that there is a deadlock in the specification.
# Let's investigate

# The following creates state space, and stores trace to deadlock.
os.system('lps2lts -v -Dt block.lps block.aut')
# Print the trace and find out what's wrong:
os.system('tracepp block.lps_dlk_0.trc')

# The same is also immediately clear if we visualise the state space
os.system('ltsgraph block.aut')


