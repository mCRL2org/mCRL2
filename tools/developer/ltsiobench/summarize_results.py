#!/usr/bin/env python3
# Author(s): Jeroen Keiren
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""Summarise the output of run_benchmarks.sh as a Markdown table.

Usage: summarize_results.py RESULTDIR [BASELINEDIR]

With BASELINEDIR, an extra column shows the speedup of the load time relative to the baseline.
"""

import json
import re
import sys
from pathlib import Path


def median_ms(result):
    """The median wall-clock time in ms; supports the hyperfine 1.x and 2.x JSON formats."""
    if "summary" in result:
        return 1000.0 * result["summary"]["time_wall_clock"]["median"]
    return 1000.0 * result["median"]


def relative_stddev(result):
    if "summary" in result:
        wall = result["summary"]["time_wall_clock"]
        return wall["stddev"] / wall["mean"]
    return result["stddev"] / result["mean"]


def median_instructions(result):
    """The median number of retired instructions (hyperfine 2.x only), or None."""
    return result.get("summary", {}).get("instructions", {}).get("median")


def read_results(directory):
    """Returns {input name: {mode: median time in ms}}, {input name: {mode: {key: value}}} and
    {input name: {mode: (relative stddev, median instructions)}}."""
    times = {}
    stats = {}
    extra = {}
    for json_file in sorted(Path(directory).glob("*.json")):
        name = json_file.stem
        data = json.loads(json_file.read_text())
        times[name] = {r.get("name", r["command"]): median_ms(r) for r in data["results"]}
        extra[name] = {r.get("name", r["command"]): (relative_stddev(r), median_instructions(r)) for r in data["results"]}

        stats[name] = {}
        stats_file = json_file.with_suffix(".stats.txt")
        if stats_file.exists():
            current = {}
            for line in stats_file.read_text().splitlines():
                m = re.match(r"([a-z_ -]+): (.*)", line)
                if m:
                    current[m.group(1)] = m.group(2)
                    if m.group(1) == "peak_rss_mib":
                        stats[name][current["mode"]] = current
                        current = {}
    return times, stats, extra


def sort_key(name):
    shape, _, size = name.partition("_")
    return (int(size.rstrip("M")) if size.rstrip("M").isdigit() else 0, shape)


def main():
    if len(sys.argv) not in (2, 3):
        print(__doc__, file=sys.stderr)
        return 2
    times, stats, extra = read_results(sys.argv[1])
    baseline = read_results(sys.argv[2])[0] if len(sys.argv) == 3 else None

    header = ["input", "MB", "raw ms", "decode ms", "load ms", "load ± %", "load − decode", "decode / load", "aut ms",
              "lts / aut", "ns / transition", "load instr. / transition", "load RSS MiB"]
    if baseline:
        header.append("speedup vs baseline")
    print("| " + " | ".join(header) + " |")
    print("|" + "|".join("---" for _ in header) + "|")

    for name in sorted(times, key=sort_key):
        t = times[name]
        s = stats.get(name, {})
        size_mb = int(s.get("raw", {}).get("bytes", 0)) / 1e6
        transitions = int(s.get("load", {}).get("transitions", 0))
        load_stddev, load_instructions = extra[name].get("load", (0.0, None))
        row = [
            name,
            f"{size_mb:.0f}",
            f"{t.get('raw', 0):.0f}",
            f"{t.get('decode', 0):.0f}",
            f"{t.get('load', 0):.0f}",
            f"{100 * load_stddev:.0f}",
            f"{t.get('load', 0) - t.get('decode', 0):.0f}",
            f"{100 * t.get('decode', 0) / t['load']:.0f}%" if t.get("load") else "",
            f"{t['aut']:.0f}" if "aut" in t else "",
            f"{t['load'] / t['aut']:.1f}×" if "aut" in t and "load" in t else "",
            f"{1e6 * t['load'] / transitions:.0f}" if transitions and "load" in t else "",
            f"{load_instructions / transitions:.0f}" if transitions and load_instructions else "",
            f"{float(s['load']['peak_rss_mib']):.0f}" if "load" in s else "",
        ]
        if baseline:
            b = baseline.get(name, {})
            row.append(f"{b['load'] / t['load']:.2f}×" if "load" in b and "load" in t else "")
        print("| " + " | ".join(row) + " |")
    return 0


if __name__ == "__main__":
    sys.exit(main())
