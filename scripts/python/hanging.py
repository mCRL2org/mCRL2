#!/usr/bin/env python

"""
A script that can take a teamcity build log and prints tests that did not
finish. Can be used to figure out hanging tests.
"""
import re
import sys

if len(sys.argv) != 2:
    print("Usage: hanging.py <build_log>")
    exit(-1)

names = dict()
tests = set()
found_tests = set()

# Example begin test
# [10:56:33] :		 [Step 1/1]         Start 516: random_pbesrewr-simplify-quantifiers-rewriter
begin_test = re.compile(r".* Start\s*([0-9]*): (.*)")

# Example finish test
# [10:56:33] :		 [Step 1/1] 510/600 Test #513: random_pbesrewr-simplify ................................................   Passed    0.90 sec
finish_test = re.compile(r".*Test.*\#([0-9]*): .*")

with open(sys.argv[1], "r", encoding="utf-8") as file:
    for line in file.readlines():
        result = begin_test.match(line)
        if result is not None:
            # Started test
            print(f"Started test {result.group(1)}")
            index = int(result.group(1))
            tests.add(index)
            names[index] = result.group(2)

        result = finish_test.match(line)
        if result is not None:
            # Finished test
            print(f"Finished test {result.group(1)}")
            tests.discard(int(result.group(1)))

print("Hanging tests...")
for test in tests:
    print(f"{test} : {names[test]}")
