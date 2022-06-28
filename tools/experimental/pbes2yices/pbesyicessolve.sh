#!/bin/bash

PBES2YICES=pbes2yices
YICES=yices

die() {
	echo "$1" >&2
	exit 1
}

verbose=false
if [[ $1 == -v ]]; then
	verbose=true
	shift
fi

pbes="$1"

pbestype=$($PBES2YICES "$pbes") || die "Input PBES $pbes is not conjunctive or disjunctive, giving up."

[[ $verbose == true ]] && [[ $pbestype == conjunctive ]] && echo "Found conjunctive PBES" >&2
[[ $verbose == true ]] && [[ $pbestype == disjunctive ]] && echo "Found disjunctive PBES" >&2

levels=1

while true; do
	[[ $verbose == true ]] && echo "Searching for witness of length $((levels * 2))..." >&2
	yicesfile=$(mktemp pbesyicessolve.XXXXXXXXXXXX)
	$PBES2YICES -g ${pbestype}Witness -l $((levels * 2)) $pbes $yicesfile || die "pbes2yices failure"
	result=$($YICES $yicesfile) || die "yices failure"
	rm -f $yicesfile
	if [[ $result == sat ]]; then
		[[ $verbose == true ]] && echo "Found!" >&2
		if [[ $pbestype == disjunctive ]]; then
			echo true
		else
			echo false
		fi
		exit 0
	fi
	
	[[ $verbose == true ]] && echo "Searching for acyclic unrolling of length $levels..." >&2
	yicesfile=$(mktemp pbesyicessolve.XXXXXXXXXXXX)
	$PBES2YICES -g ${pbestype}AcyclicUnrolling -l $levels $pbes $yicesfile || die "pbes2yices failure"
	result=$($YICES $yicesfile) || die "yices failure"
	rm -f $yicesfile
	if [[ $result == unsat ]]; then
		[[ $verbose == true ]] && echo "Not found!" >&2
		if [[ $pbestype == disjunctive ]]; then
			echo false
		else
			echo true
		fi
		exit 0
	fi
	
	levels=$((levels * 2))
done
