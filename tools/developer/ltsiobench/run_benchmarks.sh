#!/usr/bin/env bash
# Author(s): Jeroen Keiren
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# Runs ltsiobench through hyperfine on every .lts file in INPUTDIR, in the modes raw, decode, load and save, and on
# the matching .aut file (if present) in mode aut. Writes one hyperfine JSON and Markdown file per input, the
# single-run statistics printed by ltsiobench, and a description of the environment into RESULTDIR.
#
# Usage: run_benchmarks.sh LTSIOBENCH INPUTDIR RESULTDIR [hyperfine options...]

set -euo pipefail

if [[ $# -lt 3 ]]; then
  echo "Usage: $0 LTSIOBENCH INPUTDIR RESULTDIR [hyperfine options...]" >&2
  exit 2
fi

bench=$1
inputdir=$2
resultdir=$3
shift 3

mkdir -p "$resultdir"
script_dir=$(cd "$(dirname "$0")" && pwd)

{
  echo "date: $(date -u +%Y-%m-%dT%H:%M:%SZ)"
  echo "host: $(uname -a)"
  if command -v sysctl > /dev/null && sysctl -n machdep.cpu.brand_string > /dev/null 2>&1; then
    echo "cpu: $(sysctl -n machdep.cpu.brand_string)"
    echo "memory_bytes: $(sysctl -n hw.memsize)"
  fi
  echo "commit: $(git -C "$script_dir" rev-parse HEAD)"
  echo "dirty: $(git -C "$script_dir" status --porcelain --untracked-files=no | wc -l | tr -d ' ') modified tracked files"
  echo "binary: $bench"
  echo "hyperfine: $(hyperfine --version)"
} > "$resultdir/environment.txt"

for lts in "$inputdir"/*.lts; do
  name=$(basename "$lts" .lts)
  commands=()
  for mode in raw decode load save; do
    commands+=(--command-name "$mode" "$bench --mode=$mode $lts")
  done
  if [[ -f "$inputdir/$name.aut" ]]; then
    commands+=(--command-name "aut" "$bench --mode=aut $inputdir/$name.aut")
  fi

  echo "== $name"
  hyperfine --warmup 1 --min-runs 5 "$@" \
    --export-json "$resultdir/$name.json" \
    --export-markdown "$resultdir/$name.md" \
    "${commands[@]}"

  # One extra run per mode to record what was read and the peak resident set size.
  {
    for mode in raw decode load save; do
      "$bench" --mode=$mode "$lts"
    done
    if [[ -f "$inputdir/$name.aut" ]]; then
      "$bench" --mode=aut "$inputdir/$name.aut"
    fi
  } > "$resultdir/$name.stats.txt"
done
