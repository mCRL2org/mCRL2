#!/usr/bin/env python3

import subprocess

for experiment in ['family_based_experiments', 'product_based_experiments']:
    for nr in range(1, 13):
        mcrl2file = ''
        if experiment == 'family_based_experiments':
            mcrl2file = f'{experiment}/formula{nr}/mp_fts_prop{nr}.mcrl2'
        else:
            mcrl2file = f'{experiment}/formula{nr}/minepump.mcrl2'

        lpsfile = f'{experiment}/formula{nr}/minepump.lps'
        mcffile = f'{experiment}/formula{nr}/prop{nr}.mcf'
        pbesfile = f'{experiment}/formula{nr}/minepump_prop{nr}.pbes'

        print(f'Linearising {mcrl2file}')
        mcrl22lps = subprocess.run(
            ['mcrl22lps', '-nf', mcrl2file], stdout=subprocess.PIPE, check=True
        )
        subprocess.run(['lpssumelm', '-', lpsfile], input=mcrl22lps.stdout, check=True)

        lps2pbes = subprocess.run(
            ['lps2pbes', '-f', mcffile, lpsfile], stdout=subprocess.PIPE, check=True
        )
        pbesconstelm = subprocess.run(
            ['pbesconstelm'], input=lps2pbes.stdout, stdout=subprocess.PIPE, check=True
        )
        subprocess.run(
            ['pbesparelm', '-', pbesfile], input=pbesconstelm.stdout, check=True
        )

        print(f'Verifying property {mcffile}')
        subprocess.run(['pbessolve', pbesfile], check=True)
