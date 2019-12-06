# Treiber stack

This directory contains the Treiber stack models with the associated properties.

* Treiber_stack_no_CAS contains a naive model that does not use any compare-and-swap operations.
* Treiber_stack_CAS contains the model using CAS as described in the paper.
* Treiber_stack_DCAS contains the model that applies the fix using DCAS as was sketched in the paper.
