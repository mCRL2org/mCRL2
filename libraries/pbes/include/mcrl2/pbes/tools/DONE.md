# PBESCEGAR Progress

## Current Implementation Status

### Session 3 - A_cap_builder Bug Fix & Architecture Improvements
✅ **MAJOR FIX**: Fixed critical hanging bug in A_cap_builder
  - Root cause: Stack-based transformation wasn't properly dispatching to pbes_expression_builder's apply() methods
  - Solution: Rewrote A_cap_builder using proper builder pattern with apply() method overrides
  - Result: A_cap transformation now completes successfully with no hanging
  
✅ **Sort Detection Enhancement**: Improved handling of pre-structured sorts
  - Added `is_already_structured_in_spec()` helper to check data specification
  - Now correctly identifies Sluice, Colour, etc. as already-structured
  - Only abstracts truly basic sorts like Pos
  - Parameters properly transformed through A_cap

✅ **Tested on Real PBES**
  - Successfully processes minisluice PBES with 2 equations
  - Correctly identifies which sorts need abstraction
  - A_cap formula transformation works correctly
  - Produces valid abstracted PBES output

⚠️ **Limitation**: Structure graph solver doesn't support data expression guards  
  - After abstraction, PBES still contains `!(s3_P == 2)` style guards
  - Structure graph v2 algorithm can't handle these complex expressions
  - Causes early failure of underapproximation (expected - leads to refinement in proper CEGAR)

### Architecture & Implementation Details

**A_cap Builder (Correct Implementation)**
- Extends pbes_expression_builder<A_cap_builder>
- Overrides apply() for: not_, and_, or_, imp, forall, exists, propositional_variable_instantiation
- Properly maps quantified variables to abstracted sorts
- Transforms exists → forall (for underapproximation)
- Updates propositional variable parameters to abstracted constructors

**Sort Abstraction Strategy**
- Collects non-CFP, non-bool, non-structured sorts
- Creates structured sorts with single constructor per abstracted sort
- Stores abstract sort name (Abs_SortId) and constructor (abs_SortId)
- Maps original → abstracted sorts for formula transformation

**Current Single-Pass Algorithm**
1. Load and normalize PBES
2. Collect sorts to abstract (non-CFP, non-bool, non-structured)
3. Create abstracted data specification with structured sorts
4. Apply A_cap transformation to all equations  
5. Attempt to solve with structure graph solver
6. (Incomplete): Iterative refinement when underapproximation fails

## Next Steps (Priority Order)

### High Priority - Critical for Functionality
1. **Implement Iterative Refinement Loop**
   - Currently iterative_refinement() is defined but not called
   - Need to: Extract abstraction into helper method
   - Call helper with progressively fewer abstracted sorts
   - Continue until TRUE or all sorts un-abstracted

2. **Handle Structure Graph Limitations**
   - Option A: Simplify guards before structure graph building
   - Option B: Use SMT solver (pbes2cvc4, pbes2yices) as fallback
   - Option C: Implement state elimination to remove data guards

3. **Compute Real Control Flow Parameters**
   - Currently all non-bool parameters treated as non-CFP
   - Should use stategraph_global_algorithm to properly identify CFP
   - CFP parameters: those appearing in state transitions, not abstractions

### Medium Priority - Correctness & Quality
4. **Guard Abstraction/Simplification**
   - Properly abstract data expressions when parameters are abstracted
   - May need to add case splits for unknown comparisons
   - Ensure safety properties hold for abstraction

5. **Edge Pruning**
   - Currently placeholder: `analyze_and_prune_edges()` does nothing
   - Should evaluate guards under abstraction
   - Prune edges where guards are definitely false

6. **Testing & Verification**
   - Test on multiple PBES files
   - Verify correctness of abstraction-refinement cycle
   - Performance benchmarking

### Lower Priority - Advanced Features  
7. Options for different solvers (SMT, BDD, etc.)
8. Counterexample analysis for refinement decisions
9. Performance optimizations
10. Configuration options for abstraction strategy

## Technical Debt & Known Issues

- **Iterator-based loops**: Must use `for(const auto& v: list)` not index-based on variable_list
- **Data specification mutation**: Must be careful when adding aliases/sorts
- **Solver compatibility**: Structure graph v2 very strict about PBES format
- **Missing: Guard rewriting**: Need to handle abstractions in data expressions

## Files Modified
- `/Users/jbooy/Documents/mCRL2/libraries/pbes/include/mcrl2/pbes/tools/pbescegar.h` - Main implementation (621 lines)
- `/Users/jbooy/Documents/mCRL2/tools/experimental/pbescegar/pbescegar.cpp` - Tool entry point
- `/Users/jbooy/Documents/mCRL2/libraries/pbes/include/mcrl2/pbes/tools/PLAN.md` - Algorithm specification

## Build & Test Commands
```bash
cd ~/Documents/mcrl2-build && make pbescegar -j8
/Users/jbooy/Documents/mcrl2-build/stage/mCRL2.app/Contents/bin/pbescegar -v -d -itext test.pbes.txt
```



