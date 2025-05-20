#!/usr/bin/env python3

# Copyright 2021-2025 Wieger Wesselink, Jeroen Keiren
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import argparse
import pathlib
import random
import re
import subprocess

def dir_path(pathstr: str) -> pathlib.Path:
    path = pathlib.Path(pathstr)
    if path.is_dir():
        return path
    else:
        raise argparse.ArgumentTypeError(f"readable_dir:{pathstr} is not a valid path")

def compute_cost(columns):
    m = len(columns)    # number of columns
    n = len(columns[0]) # number of rows
    result = 0
    for i in range(n):
        for j in range(m):
            if columns[j][i] == '0':
                result = result + 1
            else:
                break
        for j in reversed(range(m)):
            if columns[j][i] == '0':
                result = result + 1
            else:
                break
    return result


def print_columns(columns):
    m = len(columns)    # number of columns
    n = len(columns[0]) # number of rows
    for i in range(n):
        row = [columns[j][i] for j in range(m)]
        print(''.join(list(map(str, row))))
    print('')


# returns a permutation of the column numbers
def maximize_cost(columns, iteration_count):
    m = len(columns)    # number of columns
    cost = compute_cost(columns)
    print(f'initial solution has {cost} zeroes')
    J = list(range(m))
    for i in range(iteration_count):
        pos1, pos2 = random.sample(J, 2)
        columns[pos1], columns[pos2] = columns[pos2], columns[pos1]
        J[pos1], J[pos2] = J[pos2], J[pos1]
        cost1 = compute_cost(columns)
        if cost1 > cost:
            cost = cost1
            print(f'found solution with {cost} zeroes')
        else:
            columns[pos1], columns[pos2] = columns[pos2], columns[pos1]
            J[pos1], J[pos2] = J[pos2], J[pos1]
    return J


def run_command(command, timeout=1000):
    import subprocess
    try:
        proc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        output, error = proc.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        proc.kill()
        return 'TimeoutExpired'
    if isinstance(output, bytes):
        output = output.decode("utf-8")
    if isinstance(error, bytes):
        error = error.decode("utf-8")
    return str(output) + str(error)

def is_used(entry: str):
    if entry == "-":
        return "0"

    return "1"

def parse_read_write_matrix(text):
    match = re.search(r'read/write patterns compacted\n(.*)\nNone?', text, flags=re.DOTALL)
    rows = [re.sub(r'^\s*\d+\s*', '', line) for line in match.group(1).split('\n')]
    n = len(rows)
    m = len(rows[0])
    columns = [[is_used(rows[i][j]) for i in range(n)] for j in range(m)]
    return columns


def run(args):
    random.seed()

    lpsreach = pathlib.Path('lpsreach')
    if args.path:
        lpsreach = args.path.joinpath(lpsreach)

    # compute a good variable ordering
    cmd = [lpsreach, '--info', f'--groups={args.groups}', args.lpsfile]
    text = run_command(cmd)
    columns = parse_read_write_matrix(text)
    permutation = maximize_cost(columns, args.iterations)

    # check the permutation
    cmd = [lpsreach, '--info', f'--groups={args.groups}', '--reorder={}'.format(' '.join(list(map(str, permutation)))), args.lpsfile]
    text = run_command(cmd)
    columns = parse_read_write_matrix(text)
    cost = compute_cost(columns)
    print(f'final solution has {cost} zeroes')

    # print the command with optimum reordering
    opts = [f'--memory-limit={args.memory_limit}', f'--rewriter={args.rewriter}', f'--threads={args.threads}', f'--groups={args.groups}']
    if args.cached:
        opts.append('--cached')
    if args.chaining:
        opts.append('--chaining')
    if args.deadlock:
        opts.append('--deadlock')
    if args.saturation:
        opts.append('--saturation')
    if args.verbose:
        opts.append('--verbose')
        opts.append('--print-nodesize')

    cmd = [lpsreach] + opts + ['--reorder={}'.format(' '.join(list(map(str, permutation)))), args.lpsfile]
    print(' '.join(map(str, cmd)))

    subprocess.run(cmd)



def main():
    cmdline_parser = argparse.ArgumentParser(description='Simple script for experimenting with variable orders.')
    cmdline_parser.add_argument('lpsfile', metavar='FILE', type=str, help='an .lps file')
    cmdline_parser.add_argument('iterations', help="the number of iterations of hill climbing", type=int, default=1000, nargs='?')

    cmdline_parser.add_argument('--cached', help="directly passed to lpsreach", action='store_true')
    cmdline_parser.add_argument('--chaining', help="directly passed to lpsreach", action='store_true')
    cmdline_parser.add_argument('--deadlock', help="directly passed to lpsreach", action='store_true')
    cmdline_parser.add_argument('--groups', help="none or simple", default='none')
    cmdline_parser.add_argument('-m', '--memory-limit', help="directly passed to lpsreach", type=int, default=3)
    cmdline_parser.add_argument('-r', '--rewriter', help="directly passed to lpsreach", type=str, default='jitty')
    cmdline_parser.add_argument('--saturation', help="directly passed to lpsreach", action='store_true')
    cmdline_parser.add_argument('--threads', help="directly passed to pbessolvesymbolic", type=int, default=1)
    cmdline_parser.add_argument('-v', '--verbose', help="directly passed to lpsreach", action='store_true')

    cmdline_parser.add_argument('--path', help="Path to the mCRL2 installation that should be used", type=dir_path)
    args = cmdline_parser.parse_args()
    run(args)


if __name__ == '__main__':
    main()
