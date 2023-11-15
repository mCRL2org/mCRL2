#!/usr/bin/env python

#~ Copyright 2016 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import argparse
import shutil
import os
import re
import sys
import time
import traceback

# Runs a command, and returns the output
def run_command(cmd, print_output = False):
    print cmd
    import subprocess
    output = subprocess.check_output(cmd, stderr=subprocess.STDOUT, shell=True)
    if print_output:
        print indent_text(output, '    ')
    return output

def position_counts(pbesfile):
    cmd = 'reduce_pbes -d {}'.format(pbesfile)
    text = run_command(cmd).strip()
    text = re.sub(r'[^[]*\[', '[', text)
    return eval(text)

def reduce_pbes(pbesfile, depth):
    cmd = 'reduce_pbes {} -D{}'.format(pbesfile, depth)
    text = run_command(cmd)
    files = re.findall('file = .*$', text, re.MULTILINE)
    files = [file[7:].strip() for file in files]
    return files

def identical_files(file1, file2):
    import filecmp
    return filecmp.cmp(file1, file2, False)

def check_invariant(script, pbesfile):
    cmd = '{} {}'.format(script, pbesfile)
    text = run_command(cmd)
    return text.strip() == 'True'

def minimize_pbes_at_depth(pbesfile, script, depth):
    files = reduce_pbes(pbesfile, depth)
    for file in files:
        if identical_files(pbesfile, file):
            continue
        if check_invariant(script, file):
            return file
    return None

def minimize_pbes(pbesfile, script, maxdepth):
    while True:
        for depth in range(0, maxdepth):
            file = minimize_pbes_at_depth(pbesfile, script, depth)
            if file:
                print 'found', file
                shutil.copy(file, pbesfile)
                break
        if not file:
            return

def remove_equations(pbesfile):
    cmd = 'pbestransform -aremove-unused-pbes-equations {0} {0}'.format(pbesfile)
    run_command(cmd).strip()

def main():
    cmdline_parser = argparse.ArgumentParser()
    cmdline_parser.add_argument('pbesfile', metavar='INFILE', type=str, help='The .pbes file that is reduced.')
    cmdline_parser.add_argument('invariant', metavar='SCRIPT', type=str, help="A script that checks an invariant on a PBES. It should write 'True' if it holds.")
    cmdline_parser.add_argument('-d', '--max-depth', metavar='DEPTH', default='6', help='The maximum depth')
    cmdline_parser.add_argument('-r', '--remove-equations', help='Remove unreachable equations', action='store_true')
    args = cmdline_parser.parse_args()

    minimize_pbes(args.pbesfile, args.invariant, int(args.max_depth))
    if args.remove_equations:
        remove_equations(args.pbesfile)

if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print 'ERROR:', e
        traceback.print_exc()
