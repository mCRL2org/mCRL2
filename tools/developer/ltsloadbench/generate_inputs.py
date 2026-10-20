#!/usr/bin/env python3
# Author(s): Jeroen Keiren
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""Generate benchmark inputs for ltsloadbench.

Every shape is a torus of N x N states with three outgoing transitions per state, so it has N^2 states and 3 N^2
transitions. The shapes differ in what the file contains besides the transitions:

  grid   three plain actions, no state labels (--no-info)        isolates transition decoding
  info   three plain actions, with state labels (2 Nat params)   adds state-label terms
  data   actions a(n), b(n), c(n) with up to K distinct values   many distinct action labels
  wide   three plain actions, 18 Nat parameters in state labels  large state-label terms

For each shape and size the script writes <shape>_<size>.mcrl2, .lps, .lts and (optionally) .aut into the
output directory.
"""

import argparse
import math
import subprocess
import sys
import time
from pathlib import Path

SIZES = {"1M": 1_000_000, "10M": 10_000_000, "50M": 50_000_000}
SHAPES = ["grid", "info", "data", "wide"]
WIDE_EXTRA_PARAMETERS = 16
DATA_LABEL_VALUES = 1000


def successors(n):
    """The three successors (i', j') of state (i, j) as mCRL2 expressions."""
    inc_i = f"(i + 1) mod {n}"
    inc_j = f"(j + 1) mod {n}"
    return [("a", inc_i, "j"), ("b", "i", inc_j), ("c", inc_i, inc_j)]


def specification(shape, n):
    if shape in ("grid", "info"):
        summands = [f"{a}.P({ni}, {nj})" for (a, ni, nj) in successors(n)]
        return (
            "act a, b, c;\n"
            "proc P(i, j: Nat) =\n    " + "\n  + ".join(summands) + ";\n"
            "init P(0, 0);\n"
        )
    if shape == "data":
        summands = [f"{a}((i + j) mod {DATA_LABEL_VALUES}).P({ni}, {nj})" for (a, ni, nj) in successors(n)]
        return (
            "act a, b, c: Nat;\n"
            "proc P(i, j: Nat) =\n    " + "\n  + ".join(summands) + ";\n"
            "init P(0, 0);\n"
        )
    if shape == "wide":
        # The extra parameters are functions of (i, j), so the state space stays N x N, but every state label
        # contains WIDE_EXTRA_PARAMETERS + 2 natural numbers.
        names = [f"v{k}" for k in range(1, WIDE_EXTRA_PARAMETERS + 1)]

        def args(ni, nj):
            extra = [f"(({ni}) + {k} * ({nj})) mod 1000" for k in range(1, WIDE_EXTRA_PARAMETERS + 1)]
            return ", ".join([ni, nj] + extra)

        # The condition is always true, but it uses the extra parameters, which keeps mcrl22lps from removing them.
        condition = " + ".join(names) + f" < {1000 * WIDE_EXTRA_PARAMETERS}"
        summands = [f"{a}.P({args(ni, nj)})" for (a, ni, nj) in successors(n)]
        return (
            "act a, b, c;\n"
            f"proc P(i, j, {', '.join(names)}: Nat) =\n  ({condition}) -> (\n    "
            + "\n  + ".join(summands)
            + ");\n"
            f"init P({args('0', '0')});\n"
        )
    raise ValueError(shape)


def run(command):
    print("+ " + " ".join(str(c) for c in command), flush=True)
    start = time.monotonic()
    subprocess.run([str(c) for c in command], check=True)
    print(f"  ({time.monotonic() - start:.1f}s)", flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("bindir", type=Path, help="directory containing mcrl22lps, lps2lts and ltsconvert")
    parser.add_argument("outdir", type=Path, help="directory to write the inputs to")
    parser.add_argument("--shapes", nargs="+", choices=SHAPES, default=SHAPES)
    parser.add_argument("--sizes", nargs="+", choices=list(SIZES), default=["1M", "10M"])
    parser.add_argument("--aut", action="store_true", help="also convert every .lts to .aut")
    parser.add_argument("--rewriter", default="jitty", help="rewriter passed to lps2lts (default: jitty)")
    arguments = parser.parse_args()

    arguments.outdir.mkdir(parents=True, exist_ok=True)
    for size in arguments.sizes:
        n = math.ceil(math.sqrt(SIZES[size] / 3))
        for shape in arguments.shapes:
            base = arguments.outdir / f"{shape}_{size}"
            mcrl2_file = base.with_suffix(".mcrl2")
            lps_file = base.with_suffix(".lps")
            lts_file = base.with_suffix(".lts")
            print(f"== {base.name}: N = {n}, {n * n} states, {3 * n * n} transitions", flush=True)
            mcrl2_file.write_text(specification(shape, n))
            run([arguments.bindir / "mcrl22lps", "-q", mcrl2_file, lps_file])
            lps2lts = [arguments.bindir / "lps2lts", "-q", f"-r{arguments.rewriter}", lps_file, lts_file]
            if shape in ("grid", "data"):
                lps2lts.insert(2, "--no-info")
            run(lps2lts)
            if arguments.aut:
                run([arguments.bindir / "ltsconvert", "-q", lts_file, base.with_suffix(".aut")])
    return 0


if __name__ == "__main__":
    sys.exit(main())
