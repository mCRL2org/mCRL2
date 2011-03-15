import os

os.system('mcrl22lps -v allow.mcrl2 allow.lps')

os.system('lps2pbes -v -f nodeadlock.mcf allow.lps allow.nodeadlock.pbes')
os.system('pbes2bool -v allow.nodeadlock.pbes')
# The above shows that there is a deadlock in the specification.
# Let's investigate

# The following creates state space, and stores trace to deadlock.
os.system('lps2lts -v -Dt allow.lps allow.aut')
# Print the trace and find out what's wrong:
os.system('tracepp allow.lps_dlk_0.trc')

# The same is also immediately clear if we visualise the state space
os.system('ltsgraph allow.aut')


