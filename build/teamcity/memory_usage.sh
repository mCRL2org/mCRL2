#!/bin/sh

# Formats the resident set output in a way that teamcity interprets it in the test output.
/usr/bin/time --format="##teamcity[testMetadata testName='package.Test.testName' name='Resident Memory Usage (KB)' type='number' value='%M']" $@
