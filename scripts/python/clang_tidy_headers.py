#!/usr/bin/env python3

"""
Run clang-tidy on every .h file found recursively in a given directory,
using a compile_commands.json located in the build directory passed via -p.

Example:
    clang_tidy_headers.py libraries/atermpp/include -p build
    clang_tidy_headers.py libraries/atermpp/include -p build -j 8 --fix
"""

import argparse
import multiprocessing
import os
import shutil
import subprocess
import sys
from pathlib import Path


def find_header_files(directory: Path) -> list[Path]:
    """Return a sorted list of all .h files under *directory* (recursive)."""
    return sorted(directory.rglob("*.h"))


def run_clang_tidy(args: tuple[str, Path, Path, list[str]]) -> tuple[Path, int, str]:
    """Run clang-tidy on a single file. Returns (path, returncode, output)."""
    clang_tidy, header, build_dir, extra_args = args
    cmd = [clang_tidy, "-p", str(build_dir), *extra_args, str(header)]
    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            check=False,
        )
    except FileNotFoundError as exc:
        return header, 127, f"Failed to invoke {clang_tidy}: {exc}"
    output = result.stdout + result.stderr
    return header, result.returncode, output


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Run clang-tidy on every .h file in a directory tree.",
        epilog="Any arguments after '--' are forwarded verbatim to clang-tidy.",
    )
    parser.add_argument(
        "directory",
        type=Path,
        help="Directory to search recursively for .h files.",
    )
    parser.add_argument(
        "-p",
        "--build-path",
        type=Path,
        required=True,
        help="Path to the build directory containing compile_commands.json.",
    )
    parser.add_argument(
        "-j",
        "--jobs",
        type=int,
        default=max(1, multiprocessing.cpu_count() // 2),
        help="Number of clang-tidy processes to run in parallel.",
    )
    parser.add_argument(
        "--clang-tidy",
        default=os.environ.get("CLANG_TIDY", "clang-tidy"),
        help="Path or name of the clang-tidy executable (default: clang-tidy).",
    )

    # Manually split argv at the first '--' so the user can pass options to
    # clang-tidy without argparse trying to interpret them.
    argv = sys.argv[1:]
    if "--" in argv:
        sep = argv.index("--")
        own_args, extra_args = argv[:sep], argv[sep + 1 :]
    else:
        own_args, extra_args = argv, []

    options = parser.parse_args(own_args)

    directory: Path = options.directory.resolve()
    build_dir: Path = options.build_path.resolve()

    if not directory.is_dir():
        print(f"error: '{directory}' is not a directory", file=sys.stderr)
        return 2

    compile_db = build_dir / "compile_commands.json"
    if not compile_db.is_file():
        print(f"error: no compile_commands.json in '{build_dir}'", file=sys.stderr)
        return 2

    if shutil.which(options.clang_tidy) is None and not Path(options.clang_tidy).is_file():
        print(f"error: cannot find clang-tidy executable '{options.clang_tidy}'", file=sys.stderr)
        return 2

    headers = find_header_files(directory)
    if not headers:
        print(f"No .h files found under '{directory}'.")
        return 0

    print(
        f"Running {options.clang_tidy} on {len(headers)} header(s) "
        f"using compile commands from '{compile_db}' with {options.jobs} job(s)."
    )

    work = [(options.clang_tidy, h, build_dir, extra_args) for h in headers]

    failures = 0
    completed = 0
    with multiprocessing.Pool(processes=options.jobs) as pool:
        try:
            for header, returncode, output in pool.imap_unordered(run_clang_tidy, work):
                completed += 1
                status = "OK" if returncode == 0 else f"FAIL ({returncode})"
                rel = header.relative_to(directory) if header.is_relative_to(directory) else header
                print(f"[{completed}/{len(headers)}] {status} {rel}")
                if output.strip():
                    print(output)
                if returncode != 0:
                    failures += 1
        except KeyboardInterrupt:
            pool.terminate()
            pool.join()
            print("Interrupted.", file=sys.stderr)
            return 130

    print(f"\nFinished: {len(headers) - failures} ok, {failures} failed.")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
