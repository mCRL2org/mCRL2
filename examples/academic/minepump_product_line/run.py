import os

verbose = False
v = ''
if verbose: 
  v = '-v' 
else: 
  v = '-q'

for experiment in ['family_based_experiments','product_based_experiments']:
  for nr in range(1,13):
  
    mcrl2file = ''
    if experiment == 'family_based_experiments':
      mcrl2file = '%s/formula%d/mp_fts_prop%d.mcrl2' %(experiment,nr,nr)
    else:
      mcrl2file = '%s/formula%d/minepump.mcrl2' %(experiment,nr)
    lpsfile   = '%s/formula%d/minepump.lps' %(experiment,nr)
    mcffile   = '%s/formula%d/prop%d.mcf' %(experiment,nr,nr)
    pbesfile  = '%s/formula%d/minepump_prop%d.pbes' %(experiment,nr,nr)
 
    linearise = 'mcrl22lps %s -nf %s | lpssumelm > %s' %(v,mcrl2file,lpsfile)
    pbesify   = 'lps2pbes -f %s %s | pbesconstelm %s | pbesparelm %s > %s' %(mcffile,lpsfile,v,v,pbesfile)
    solve     = 'pbespgsolve %s -rjittyc  -srecursive %s' %(v,pbesfile)

    print 'Linearising %s' %(mcrl2file)  
    os.system(linearise)
    os.system(pbesify)
    print 'Verifying property %s' %(mcffile)  
    os.system(solve)

