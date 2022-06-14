#!/bin/bash

PBES2CVC4=pbes2cvc4
CVC4=cvc4

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

pbestype=$($PBES2CVC4 "$pbes") || die "Input PBES $pbes is not conjunctive or disjunctive, giving up."

[[ $verbose == true ]] && [[ $pbestype == conjunctive ]] && echo "Found conjunctive PBES" >&2
[[ $verbose == true ]] && [[ $pbestype == disjunctive ]] && echo "Found disjunctive PBES" >&2

levels=1

while true; do
	[[ $verbose == true ]] && echo "Searching for witness of length $((levels * 2))..." >&2
	cvc4file=$(mktemp pbescvc4solve.XXXXXXXXXXXX.smt2)
	$PBES2CVC4 -g ${pbestype}Witness -l $((levels * 2)) $pbes $cvc4file || die "pbes2cvc4 failure"
	result=$($CVC4 $cvc4file) || die "cvc4 failure"
	rm -f $cvc4file
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
	cvc4file=$(mktemp pbescvc4solve.XXXXXXXXXXXX.smt2)
	$PBES2CVC4 -g ${pbestype}AcyclicUnrolling -l $levels $pbes $cvc4file || die "pbes2cvc4 failure"
	result=$($CVC4 $cvc4file) || die "cvc4 failure"
	rm -f $cvc4file
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
