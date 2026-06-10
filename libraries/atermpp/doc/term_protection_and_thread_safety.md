# Term protection and thread safety in atermpp

This document describes how protected and unprotected terms work in the
`atermpp` library, how the garbage collector finds its root set, and how the
per-thread bookkeeping can be corrupted — in particular when a term variable is
destroyed on a different thread than the one that created it.

All paths below are relative to the repository root.

## 1. The term pool in a nutshell

Every term is a node of type `detail::_aterm`
(`libraries/atermpp/include/mcrl2/atermpp/detail/aterm_core.h`): a function
symbol followed by its arguments, allocated inside the global
`detail::aterm_pool` (`libraries/atermpp/include/mcrl2/atermpp/detail/aterm_pool.h`).
Terms are *hash-consed*: each distinct term exists exactly once, so equality is
pointer comparison. Because terms are maximally shared, no single owner can
free them; instead the pool runs a stop-the-world **mark-sweep garbage
collector**:

1. **Mark**: walk a *root set* of live term pointers and set the mark bit of
   every reachable `_aterm` (the mark bit is a tag bit inside the node's
   function symbol pointer, see `_aterm::mark()`).
2. **Sweep**: every unmarked node in every storage is destroyed and its slot
   returned to the block allocator; marked nodes are unmarked again.

Everything in this document is about how the root set is maintained, because
that is where both the protected/unprotected distinction and the thread-safety
pitfalls live.

## 2. Protected terms: `aterm_core` and the protection sets

`aterm_core` (`libraries/atermpp/include/mcrl2/atermpp/aterm_core.h`) is the
base of all user-visible term types (`aterm`, `aterm_int`, `term_list`, and all
generated AST classes). It contains a single data member, the pointer
`m_term`, inherited from `unprotected_aterm_core`. What makes it *protected*
is its constructor and destructor
(`libraries/atermpp/include/mcrl2/atermpp/detail/aterm_implementation.h`):

```cpp
inline aterm_core::aterm_core() noexcept
{
  detail::g_thread_term_pool().register_variable(this);
}

inline aterm_core::~aterm_core() noexcept
{
  detail::g_thread_term_pool().deregister_variable(this);
}
```

Two things are essential here:

- **The protection set stores the address of the variable, not the term.**
  `register_variable(this)` inserts the *location* `aterm_core*` into a hash
  table (`m_variables` in `detail::thread_aterm_pool`). During marking the
  collector dereferences that location to find whatever term the variable
  holds *at that moment*. This is why assignment does not need to touch the
  protection set at all: `operator=` only overwrites `m_term` under a shared
  lock. Construction and destruction are the only operations that mutate the
  set.

- **The protection set is thread-local.** `g_thread_term_pool()`
  (`libraries/atermpp/source/aterm_implementation.cpp`) returns a
  `thread_local thread_aterm_pool`. A variable is therefore registered in the
  protection set *of the thread that runs its constructor*, and deregistered
  from the protection set *of the thread that runs its destructor*. Nothing
  checks that these are the same set — see section 6.

Every copy/move constructor registers the new object, and every assignment
(copy and move alike) merely acquires a shared lock and overwrites the
pointer. Note that a *moved-from* term is still registered; moving transfers
the value, never the registration. `swap()` exchanges pointers without any
bookkeeping, which is why it is the cheapest way to exchange terms.

The class comment on `aterm_core` states the invariant the collector relies
on: every accessible term is reachable from an address in some protection set
(or from a registered container), and during garbage collection this situation
is *stable* — no registrations, deregistrations, or assignments can happen
while marking and sweeping are in progress.

## 3. Unprotected terms: `unprotected_aterm_core`

`unprotected_aterm_core` is a plain wrapper around a `const detail::_aterm*`.
Copying or destroying it performs no bookkeeping whatsoever, so it is free —
and it provides no protection: if a garbage collection runs while an
unprotected term is the only reference, the underlying node is swept and the
pointer dangles.

Using an unprotected term is sound only while *something else* keeps the term
alive across every potential GC point:

- it refers to a subterm of a term that is protected elsewhere (subterms of a
  marked term are always marked);
- it lives inside a registered `aterm_container` (section 4), which marks it
  on the container's behalf;
- it is used inside a shared-locked section, during which no collection can
  start (this is what the internal `create_*` functions do with their argument
  arrays);
- it refers to a term that is protected by a global/static variable for the
  whole program run (e.g. the empty list).

A garbage collection can be triggered by *any* term creation on *any* thread,
so "no GC point" effectively means "no term is created and no explicit
`collect()` happens anywhere" — an assumption that is almost impossible to
maintain outside the situations above. The address-based `operator<=>` of
unprotected terms carries the same caveat: orderings are only stable as long
as the involved terms are not collected.

## 4. Containers: `aterm_container` and `reference_aterm`

Registering every element of a vector individually would make the protection
hash table churn on every `push_back`. Instead, the GC-aware containers in
`atermpp::standard_containers` store their elements as
`detail::reference_aterm<T>` — which derives from `unprotected_aterm_core` —
and register a single `detail::aterm_container`
(`libraries/atermpp/include/mcrl2/atermpp/detail/aterm_container.h`) holding a
mark function that traverses the container during GC. Like a variable, a
container is registered in the **thread-local** `m_containers` table on
construction and deregistered on destruction
(`aterm_implementation.h`, `aterm_container::aterm_container/~aterm_container`),
so everything said in section 6 about cross-thread destruction applies to
containers as well.

This is also why the project rule exists: **never store terms in plain STL
containers**. A `std::vector<aterm>` registers and deregisters every element
individually (slow, and reallocation churns the protection set), whereas
`atermpp::vector<T>` registers one container and stores unprotected elements
that are marked in place.

## 5. Thread-local pools and stop-the-world collection

Each thread owns a `detail::thread_aterm_pool`
(`libraries/atermpp/include/mcrl2/atermpp/detail/thread_aterm_pool.h`)
containing:

- `m_variables` / `m_containers`: the protection sets described above;
- a `mcrl2::utilities::shared_mutex` participating in the global
  busy/forbidden protocol;
- a `thread_aterm_pool_interface` registered with the global `aterm_pool`, by
  which the pool exposes its `mark()` function to the collector.

The `shared_mutex`
(`libraries/utilities/include/mcrl2/utilities/shared_mutex.h`) is not a
conventional reader-writer lock. Every thread's instance shares a common
`shared_mutex_data` (one `std::mutex` plus the list of all instances), and
each instance has its own atomic `busy` and `forbidden` flags:

- **Shared (read) lock** — taken for every term creation, assignment, and
  protection-set update: set *your own* busy flag; if your forbidden flag is
  raised, back off and wait until the exclusive holder releases the global
  mutex. On the fast path this is one uncontended atomic store, which is what
  makes term operations cheap in multithreaded builds.
- **Exclusive lock** — taken for garbage collection and hash table resizing:
  lock the global mutex, raise the forbidden flag of every other instance,
  then spin until each of their busy flags drops. From that point every other
  thread is parked outside any shared section: the world is stopped.

Collection itself (`aterm_pool::collect_impl` in
`libraries/atermpp/include/mcrl2/atermpp/detail/aterm_pool_implementation.h`)
runs entirely on the collecting thread: it iterates over **all** registered
thread pools, invoking each pool's `mark()` (which walks that thread's
`m_variables` and `m_containers` tables), then sweeps all storages. Reading
the other threads' tables without their cooperation is safe only because of
the stop-the-world guarantee — every mutation of a protection set happens
under a shared lock, and no thread can hold one while the collector holds the
exclusive lock.

Two consequences worth knowing:

- A thread that blocks (I/O, an OS mutex, a long external call) **while
  holding a shared lock** stalls every garbage collection and resize in the
  process, because the exclusive locker spin-waits on its busy flag.
- Without `MCRL2_ENABLE_MULTITHREADING` all of this synchronisation compiles
  away (`GlobalThreadSafe == false`) and there is exactly one "thread" pool;
  creating threads that touch terms in such a build is immediately undefined
  behaviour.

## 6. How thread-safety issues arise

### 6.1 The bookkeeping invariant

For every live protected variable `v` exactly one protection set must contain
`&v`: the set of the thread that executed `v`'s constructor. The destructor,
however, erases `&v` from the set of the thread that happens to execute *it*.
The library has no way to detect a mismatch:

- `hashtable::erase`
  (`libraries/utilities/include/mcrl2/utilities/detail/hashtable.h`) contains
  an explicit "safety escape": when the key is not found after scanning the
  whole table, it **silently returns**. A cross-thread destruction therefore
  produces no error — only a full-table scan and a no-op.
- `hashtable::insert` does not check for duplicate keys either, so a stale
  entry whose address gets reused never trips an assertion.

Everything below follows from this one asymmetry.

### 6.2 Destroying a term on a different thread than created it

Consider the common pattern of moving work to a thread pool:

```cpp
atermpp::aterm t = make_term();                 // registered in A's set
queue.push([t]() { use(t); });                  // capture copy: also registered in A
// worker thread B pops the std::function, runs it, and destroys it
```

The lambda's captured `aterm` is constructed (and registered) on thread A, but
the `std::function` — and with it the capture — is destroyed on worker thread
B. Step by step:

1. **B's deregistration is a no-op.** `deregister_variable(&v)` erases from
   *B's* table, where `&v` was never inserted. The erase scans B's entire
   table (an O(capacity) full scan, a performance bug in itself) and silently
   gives up.
2. **A's table now contains a dangling root.** The entry `&v` in A's
   protection set points to memory that no longer holds an `aterm_core` — a
   destroyed capture block, a popped stack frame, freed heap.
3. **The next GC dereferences the dangling root.** `thread_aterm_pool::mark()`
   does `detail::address(*variable)` on every registered location. Reading
   freed memory is undefined behaviour with three typical outcomes:
   - a crash in the marking phase (the "lucky" case — at least it is loud);
   - the garbage bits are interpreted as an `_aterm*` and
     `is_marked()`/`mark_term()` chase a wild pointer — a segfault or silent
     heap corruption *inside the collector*, far away from the buggy code;
   - the stale memory still happens to contain the old pointer value, the dead
     term is marked, and everything appears to work — until the allocator
     reuses the memory and the symptom shows up weeks later.

   The `variable != nullptr` / `term != nullptr` checks in `mark()` do not
   help: freed memory is rarely zeroed.

4. **Address reuse smears the corruption around.** Sooner or later A
   constructs a new term variable at the recycled address `&v`. Registration
   inserts the address *a second time* into A's table (no duplicate check).
   Destruction removes one of the two copies, leaving the other dangling
   again. Worse, the dangling entry can also be *wrongly removed*: if a
   variable created on B at address `&v` is later destroyed on A, A's erase
   finds the stale `&v` entry and removes it — now B's table holds the
   dangling root instead. Once terms migrate between threads, dangling and
   duplicated entries hop between protection sets, and the eventual crash has
   no spatial or temporal relation to the original bug.

The same applies to `atermpp` containers: an `atermpp::vector` member of an
object that is created on one thread and destroyed on another leaves a
dangling `aterm_container*` in the creator's `m_containers` table, and the
collector then calls a mark `std::function` on a destroyed object.

Note that the failure has nothing to do with the term *value* — terms
themselves are global and shared. The corruption is purely in the
variable-address bookkeeping. Sending the value of a term to another thread is
fine; what must stay thread-confined is the *lifetime of the protected
variable object*.

### 6.3 Variables that outlive their thread

The reverse problem: thread B creates a protected variable, B exits, and the
variable lives on. When a worker thread exits, its `thread_aterm_pool` (a
`thread_local`) is destroyed, and historically it deleted its protection
tables outright. Any still-registered variable then lost GC protection
entirely: the next collection swept its term even though the variable was
perfectly reachable — a use-after-free on the *term* rather than on the
bookkeeping.

The canonical victims are function-scope statics. Many headers in the data
library contain patterns like

```cpp
static atermpp::aterm sort = ...;   // initialised by whichever thread gets here first
```

If a worker thread is the first to call such a function, the static is
registered in the *worker's* protection set, but it is destroyed at program
exit on the *main* thread — long after the worker and its tables are gone.

The current code mitigates exactly this case: `~thread_aterm_pool`
(`libraries/atermpp/include/mcrl2/atermpp/detail/thread_aterm_pool.h`)
transfers all entries that are still registered at thread exit into the main
thread's pool (`g_main_thread_pool`, set on first use in
`libraries/atermpp/source/aterm_implementation.cpp`) before unregistering:

- the terms stay reachable for the collector after the worker is gone, and
- the later destruction on the main thread now finds the entry in the main
  pool's table, so the erase balances.

The main thread's own pool is deliberately leaked at process exit to dodge the
static-destruction-order fiasco.

Limits of the transfer worth keeping in mind:

- It only runs at *thread exit*. A variable destroyed on the wrong thread
  while its creator is still running (section 6.2) is not helped at all.
- It assumes transferred variables are subsequently destroyed **on the main
  thread** (true for statics destroyed at exit). If a transferred variable is
  destroyed on some third thread instead, the dangling-entry scenario returns.
- The transfer itself calls `register_variable` on the *main thread's* pool
  from the dying worker thread. The shared lock it takes is on the main
  thread's `shared_mutex`, whose busy/forbidden protocol assumes a single
  owning thread (`m_lock_depth` is a non-atomic counter). This is benign when
  the main thread is blocked in `join()` — the usual case — but a detached
  thread dying while the main thread concurrently performs term operations
  races on the main pool's lock state and hash table.

### 6.4 Related hazards

- **`thread_local` destruction order.** A second `thread_local` object that
  contains terms and is destroyed *after* the thread's `thread_aterm_pool`
  will call `deregister_variable` through a destroyed pool — undefined
  behaviour. Keep terms out of other `thread_local` objects.
- **Holding unprotected pointers across GC points** (section 3) — strictly a
  single-thread bug, but multithreading makes it much easier to hit, because
  *another* thread's term creation can trigger a collection at any moment.
  With only unprotected references in hand there is no safe window at all in a
  multithreaded build.
- **Hash-consing resurrection.** Because terms are maximally shared, a swept
  node's slot can be reused by a *different* term. A dangling unprotected
  pointer therefore does not necessarily crash; it may silently denote another
  valid term, turning a lifetime bug into wrong results.

## 7. Rules of thumb and debugging

Rules that keep the bookkeeping sound:

1. Destroy every protected variable and every `atermpp` container on the
   thread that constructed it. Watch out for hidden cross-thread destruction:
   lambdas captured into `std::function`s executed elsewhere, futures and
   promises, queues of objects containing terms, and class members of objects
   handed to other threads.
2. To pass a term between threads, keep a protected owner alive on the sending
   side for the duration of the handoff (or rely on a global/static owner),
   transfer the raw value, and have the receiver immediately store it in its
   own protected variable. The receiver's copy registers in the receiver's
   set, so its destruction there is correct.
3. Never create threads that touch terms in a build without
   `MCRL2_ENABLE_MULTITHREADING`.

Tools for hunting violations (flags in
`libraries/atermpp/include/mcrl2/atermpp/detail/aterm_configuration.h`):

- `EnableGCStressThread` spawns a dedicated background thread
  (`libraries/atermpp/source/gc_stress_thread.cpp`) that calls `collect()` in
  a loop, shrinking the window between a bookkeeping violation and the
  collection that exposes it from "occasionally" to "almost immediately".
- `EnableAggressiveGarbageCollection` forces a collection after every term
  creation — the single-threaded counterpart of the same idea.
- The ThreadSanitizer build (`MCRL2_ENABLE_THREADSANITIZER=ON`) catches data
  races on the lock protocol and the hash tables; AddressSanitizer catches the
  use-after-free reads in `mark()`.
- `EnableVariableRegistrationMetrics` reports protection-set sizes per thread,
  useful for spotting tables that grow without bound (a symptom of erases
  failing on the wrong thread).
