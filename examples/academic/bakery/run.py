#!/usr/bin/env', 'python3

import subprocess

mcrl22lps = subprocess.run(['mcrl22lps', '-v', '-f', '-D', '-n', '-w', 'bakery.mcrl2'], stdout=subprocess.PIPE, shell=True, check=True)
lpsconstelm = subprocess.run(['lpsconstelm'], input=mcrl22lps.stdout, stdout=subprocess.PIPE, shell=True, check=True)
subprocess.run(['lpsparelm', '-', 'bakery.lps'], input=lpsconstelm.stdout,  shell=True, check=True)

for formula in [
    'nodeadlock',
    'request_can_eventually_enter',
    'request_must_eventually_enter',
    'mutual_exclusion',
    'always_can_get_number',
    'get_at_least_number_circulating',
]:
    lps2pbes = subprocess.run(['lps2pbes', '-v', '-f', f'{formula}.mcf', 'bakery.lps'], stdout=subprocess.PIPE, shell=True, check=True)
    pbesrewr = subprocess.run(['pbesrewr', '-psimplify'], input=lps2pbes.stdout, stdout=subprocess.PIPE, shell=True, check=True)
    pbesconstelm = subprocess.run(['pbesconstelm'], input=pbesrewr.stdout, stdout=subprocess.PIPE, shell=True, check=True)
    subprocess.run(['pbesparelm', '-', f'bakery.{formula}.pbes'], input=pbesconstelm.stdout, stdout=subprocess.PIPE, shell=True, check=True)
    
    # Does not terminate since the statespace is infinite
    #subprocess.run(['pbes2bool', '-v', '-s3', '-rjitty', f'bakery.{formula}.pbes'], shell=True, check=True)

    # Instead we can abstract the data types to obtain over and underapproximations. These use the partial solvers of pbes2bool to terminate early.
    print('Answer by under-approximation: ')
    subprocess.run(['pbesabsinthe', '-sunder', '-a', 'abstraction.txt', f'bakery.{formula}.pbes', f'bakery.{formula}.absinthe.pbes'], shell=True, check=True)
    subprocess.run(['pbes2bool', '-v', '-s3', '-rjitty', f'bakery.{formula}.absinthe.pbes'], shell=True, check=True)

    print('Answer by over-approximation: ')
    subprocess.run(['pbesabsinthe', '-sover', '-a', 'abstraction.txt', f'bakery.{formula}.pbes', f'bakery.{formula}.absinthe.pbes'], shell=True, check=True)
    subprocess.run(['pbes2bool', '-v', '-s3', '-rjitty', f'bakery.{formula}.absinthe.pbes'], shell=True, check=True)

    # Alternatively, we can abstract more aggressive to ensure that the parity game is finite.
    print('Answer by under-approximation: ')
    subprocess.run(['pbesabsinthe', '-sunder', '-a', 'aggressive_abstraction.txt', f'bakery.{formula}.pbes', f'bakery.{formula}.absinthe.pbes'], shell=True, check=True)
    subprocess.run(['pbes2bool', '-v', '-rjitty', f'bakery.{formula}.absinthe.pbes'], shell=True, check=True)

    print('Answer by over-approximation: ')
    subprocess.run(['pbesabsinthe', '-sover', '-a', 'aggressive_abstraction.txt', f'bakery.{formula}.pbes', f'bakery.{formula}.absinthe.pbes'], shell=True, check=True)
    subprocess.run(['pbes2bool', '-v', '-rjitty', f'bakery.{formula}.absinthe.pbes'], shell=True, check=True)