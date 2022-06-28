#!/usr/bin/env python3

# Copyright 2021 Wieger Wesselink.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import argparse
import os
import random
import re


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
    print('initial solution has {} zeroes'.format(cost))
    J = list(range(m))
    for i in range(iteration_count):
        pos1, pos2 = random.sample(J[1:], 2) # N.B. the first element should not be moved
        columns[pos1], columns[pos2] = columns[pos2], columns[pos1]
        J[pos1], J[pos2] = J[pos2], J[pos1]
        cost1 = compute_cost(columns)
        if cost1 > cost:
            cost = cost1
            print('found solution with {} zeroes'.format(cost))
        else:
            columns[pos1], columns[pos2] = columns[pos2], columns[pos1]
            J[pos1], J[pos2] = J[pos2], J[pos1]
    return J


def run_command(command_line, timeout=1000):
    import shlex
    import subprocess
    try:
        command = shlex.split(command_line)
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


def parse_read_write_matrix(text):
    m = re.search(r'used parameters\s*(.*?)\s*read', text, flags=re.DOTALL)
    rows = [re.sub(r'^\s*\d+\s*', '', line) for line in m.group(1).split('\n')]
    n = len(rows)
    m = len(rows[0])
    columns = [[rows[i][j] for i in range(n)] for j in range(m)]
    return columns


def run(args):
    random.seed()

    # compute a good variable ordering
    cmd = 'pbessolvesymbolic --info --groups={} {}'.format(args.groups, args.pbesfile)
    text = run_command(cmd)
    columns = parse_read_write_matrix(text)
    permutation = maximize_cost(columns, args.iterations)

    # check the permutation
    cmd = 'pbessolvesymbolic --info --groups={} --reorder="{}" {}'.format(args.groups, ' '.join(list(map(str, permutation))), args.pbesfile)
    text = run_command(cmd)
    columns = parse_read_write_matrix(text)
    cost = compute_cost(columns)
    print('final solution has {} zeroes'.format(cost))

    # print the command with optimum reordering
    cmd = 'pbessolvesymbolic -v --groups={} --reorder="{}" {}'.format(args.groups, ' '.join(list(map(str, permutation))), args.pbesfile)
    print(cmd)
    os.system(cmd)


def main():
    cmdline_parser = argparse.ArgumentParser(description='Simple script for experimenting with variable orders.')
    cmdline_parser.add_argument('pbesfile', metavar='FILE', type=str, help='a .pbes file')
    cmdline_parser.add_argument('iterations', help="the number of iterations of hill climbing", type=int, default=1000, nargs='?')
    cmdline_parser.add_argument('--groups', help="none or simple", default='none')
    args = cmdline_parser.parse_args()
    run(args)


if __name__ == '__main__':
    main()
