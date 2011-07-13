# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# Random tests

add_test(test_pbesconstelm_random ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/libraries/core/build/test_pbesconstelm.py -t ${CMAKE_BINARY_DIR}/tools)
add_test(test_pbesinst_finite_random ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/libraries/core/build/test_pbesinst_finite.py -t ${CMAKE_BINARY_DIR}/tools)
add_test(test_pbesinst_lazy_random ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/libraries/core/build/test_pbesinst_lazy.py -t ${CMAKE_BINARY_DIR}/tools)
add_test(test_pbesparelm_random ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/libraries/core/build/test_pbesparelm.py -t ${CMAKE_BINARY_DIR}/tools)
add_test(test_pbes_solvers_random ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/libraries/core/build/test_pbes_solvers.py -t ${CMAKE_BINARY_DIR}/tools)
add_test(test_pfnf_rewriter_random ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/libraries/core/build/test_pfnf_rewriter.py -t ${CMAKE_BINARY_DIR}/tools)

if(MCRL2_ENABLE_EXPERIMENTAL)
  add_test(test_bes_solvers_random ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/libraries/core/build/test_bes_solvers.py -t ${CMAKE_BINARY_DIR}/tools)
  add_test(test_pbesabstract_random ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/libraries/core/build/test_pbesabstract.py -t ${CMAKE_BINARY_DIR}/tools)
  add_test(test_pbespareqelm_random ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/libraries/core/build/test_pbespareqelm.py -t ${CMAKE_BINARY_DIR}/tools)
endif(MCRL2_ENABLE_EXPERIMENTAL)

