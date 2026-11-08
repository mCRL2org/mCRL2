// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_SYMBOLIC_SYMBOLIC_REACHABILITY_H
#define MCRL2_SYMBOLIC_SYMBOLIC_REACHABILITY_H

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/data/consistency.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/find_quantifier_variables.h"
#include "mcrl2/data/substitution_utility.h"
#include "mcrl2/data/substitutions/mutable_indexed_substitution.h"
#include "mcrl2/data/undefined.h"
#include "mcrl2/symbolic/alternative_relprod.h"
#include "mcrl2/symbolic/summand_group.h"
#include "mcrl2/utilities/stopwatch.h"

#include <sylvan_ldd.hpp>

#include <atomic>
#include <exception>
#include <memory>
#include <mutex>

namespace mcrl2::symbolic {

struct symbolic_reachability_options
{
  data::rewrite_strategy rewrite_strategy = data::rewrite_strategy::jitty;
  std::size_t max_workers = 0;
  std::size_t max_iterations = 0;
  bool cached = false;
  bool chaining = false;
  bool detect_deadlocks = false;
  bool one_point_rule_rewrite = false;
  bool print_exact = false;
  bool print_nodesize = false;
  bool replace_constants_by_variables = false;
  bool remove_unused_rewrite_rules = false;
  bool saturation = false;
  bool no_discard = false;
  bool no_discard_read = false;
  bool no_discard_write = false;
  bool no_relprod = false;
  bool parallel_groups = false; // apply the transition groups in parallel (only without chaining and saturation)
  bool parallel_learning = false; // learn the transitions of a group in parallel (all modes)
  bool info = false;
  bool replace_dont_care = false;
  std::string summand_groups = "none";
  std::string variable_order = "none";
  std::string dot_file;
};

inline
std::ostream& operator<<(std::ostream& out, const symbolic_reachability_options& options)
{
  out << "rewrite-strategy = " << options.rewrite_strategy << std::endl;
  out << "cached = " << std::boolalpha << options.cached << std::endl;
  out << "chaining = " << std::boolalpha << options.chaining << std::endl;
  out << "detect_deadlocks = " << std::boolalpha << options.detect_deadlocks << std::endl;
  out << "one-point-rule-rewrite = " << std::boolalpha << options.one_point_rule_rewrite << std::endl;
  out << "print_nodesize = " << std::boolalpha << options.print_nodesize << std::endl;
  out << "replace-constants-by-variables = " << std::boolalpha << options.replace_constants_by_variables << std::endl;
  out << "remove-unused-rewrite-rules = " << std::boolalpha << options.remove_unused_rewrite_rules << std::endl;
  out << "saturation = " << std::boolalpha << options.saturation << std::endl;
  out << "no-discard = " << std::boolalpha << options.no_discard << std::endl;
  out << "no-read = " << std::boolalpha << options.no_discard_read << std::endl;
  out << "no-write = " << std::boolalpha << options.no_discard_write << std::endl;
  out << "no-relprod = " << std::boolalpha << options.no_relprod << std::endl;
  out << "parallel-groups = " << std::boolalpha << options.parallel_groups << std::endl;
  out << "parallel-learning = " << std::boolalpha << options.parallel_learning << std::endl;
  out << "info = " << std::boolalpha << options.info << std::endl;
  out << "groups = " << options.summand_groups << std::endl;
  out << "reorder = " << options.variable_order << std::endl;
  out << "dot = " << options.dot_file << std::endl;
  return out;
}

// Add operations on reals that are needed for the exploration.
inline
std::set<data::function_symbol> add_real_operators(std::set<data::function_symbol> s)
{
  std::set<data::function_symbol> result = std::move(s);
  result.insert(data::less_equal(data::sort_real::real_()));
  result.insert(data::greater_equal(data::sort_real::real_()));
  result.insert(data::sort_real::plus(data::sort_real::real_(), data::sort_real::real_()));
  return result;
}

inline
data::rewriter construct_rewriter(const data::data_specification& dataspec, data::rewrite_strategy rewrite_strategy, const std::set<data::function_symbol>& function_symbols, bool remove_unused_rewrite_rules)
{
  if (remove_unused_rewrite_rules)
  {
    return data::rewriter(dataspec,
                          data::used_data_equation_selector(dataspec, add_real_operators(function_symbols), std::set<data::variable>(), false),
                          rewrite_strategy);
  }
  else
  {
    return data::rewriter(dataspec, rewrite_strategy);
  }
}

template <typename EnumeratorElement>
void check_enumerator_solution(const EnumeratorElement& p, const summand_group&)
{
  if (p.expression() != data::sort_bool::true_())
  {    
    if (!data::find_quantifier_variables(p.expression()).empty()) 
    {
      mCRL2log(log::log_level_t::info) << "The condition contains quantifiers, which means that rewriting to normal form could fail when the quantifier enumeration limit is too low. Use ---qlimit/-Q to increase the limit." << std::endl;
    }

    throw data::enumerator_error("Expression does not rewrite to true or false: " + data::pp(p.expression()));
  }
}

/// \brief What one Lace worker needs to learn transitions by itself in parallel: the rewriter, the substitution and
/// the enumerator are not thread-safe, so every worker has its own.
/// \details A worker makes its learner the first time it learns, and keeps it in a thread_local slot until the
///          worker thread ends (or until a learning pass of another algorithm instance needs a new one). The term
///          library protects terms per thread, so the learner must be made and destroyed on the same thread.
struct worker_learner
{
  std::size_t generation;                 // the algorithm instance that this learner belongs to
  data::data_specification dataspec;     // a copy: the enumerator refers to it, and it must outlive the algorithm
  data::rewriter rewr;
  data::mutable_indexed_substitution<> sigma;
  data::enumerator_identifier_generator id_generator;
  data::enumerator_algorithm<> enumerator;

  worker_learner(std::size_t generation_, const data::data_specification& dataspec_, data::rewriter rewr_,
                 const data::mutable_indexed_substitution<>& sigma_, std::size_t worker)
    : generation(generation_),
      dataspec(dataspec_),
      rewr(std::move(rewr_)),
      sigma(sigma_),
      id_generator("w" + std::to_string(worker) + "_"),
      enumerator(rewr, dataspec, rewr, id_generator, false)
  {
    rewr.thread_initialise();
  }
};

/// \brief The learner of the calling thread (see worker_learner).
inline std::unique_ptr<worker_learner>& thread_worker_learner()
{
  // Thread-local objects are destroyed in the reverse order of their construction, so first make sure that this
  // thread's term pool exists: then it outlives the learner, whose terms it must still protect when the learner dies.
  atermpp::detail::g_thread_term_pool();
  thread_local std::unique_ptr<worker_learner> learner;
  return learner;
}

/// \brief A new number for each algorithm instance, to tell the learners of the workers apart.
inline std::size_t new_learner_generation()
{
  static std::atomic<std::size_t> generation{0};
  return ++generation;
}

/// \brief The learning statistics of one worker, on a cache line of its own (workers update them for every state).
struct alignas(64) worker_learn_statistics
{
  double time = 0.0;
  std::size_t calls = 0;
};

/// \brief The rewriter, substitution and enumerator with which a callback learns.
struct learn_resources
{
  data::mutable_indexed_substitution<>& sigma;
  const data::rewriter& rewr;
  const data::enumerator_algorithm<>& enumerator;
};

/// \brief The context of learning the transitions of one summand group, see learn_successors.
/// \details Sequentially, the callback uses the rewriter, substitution and enumerator of the algorithm. In parallel,
///          every Lace worker uses its own worker_learner, made from (a clone of) those of the algorithm.
template <typename Algorithm, typename Group>
struct learn_context
{
  Algorithm& algorithm;
  Group& group;
  bool parallel;
  bool no_relprod;
  std::size_t generation;                       // of the algorithm instance
  data::rewriter& rewr;                         // cloned by every worker for its learner, in parallel
  data::mutable_indexed_substitution<>& sigma;
  const data::enumerator_algorithm<>& enumerator;
  const data::data_specification& dataspec;
  std::size_t index_threads;                    // the number of threads of the data and action indices
  std::vector<worker_learn_statistics> statistics; // per worker
  std::atomic<bool> failed{false};
  std::mutex error_mutex;
  std::exception_ptr error; // the first exception of a callback; learn_successors rethrows it

  learn_context(Algorithm& algorithm_, Group& group_, const symbolic_reachability_options& options,
                std::size_t generation_, data::rewriter& rewr_, data::mutable_indexed_substitution<>& sigma_,
                const data::enumerator_algorithm<>& enumerator_, const data::data_specification& dataspec_)
    : algorithm(algorithm_), group(group_), parallel(options.parallel_learning), no_relprod(options.no_relprod),
      generation(generation_), rewr(rewr_), sigma(sigma_), enumerator(enumerator_), dataspec(dataspec_),
      index_threads(options.parallel_learning ? lace_workers() : 1), statistics(lace_workers())
  {}

  /// \brief The thread index for the indexed sets: 0 with one thread, worker + 1 otherwise (see utilities::indexed_set).
  std::size_t thread_index(const WorkerP* worker) const
  {
    return index_threads == 1 ? 0 : worker->worker + 1;
  }

  /// \brief The rewriter, substitution and enumerator for the calling worker; in parallel made on its first use.
  learn_resources resources(const WorkerP* worker)
  {
    if (!parallel)
    {
      return {sigma, rewr, enumerator};
    }

    std::unique_ptr<worker_learner>& learner = thread_worker_learner();
    if (!learner || learner->generation != generation)
    {
      // Copying the rewriter, substitution and data specification reads (and may update caches of) the shared
      // originals, so workers make their learners one at a time. This covers no Lace or Sylvan operation.
      static std::mutex creation_mutex;
      std::scoped_lock guard(creation_mutex);
      learner.reset();
      learner = std::make_unique<worker_learner>(generation, dataspec, rewr.clone(), sigma, worker->worker);
    }
    return {learner->sigma, learner->rewr, learner->enumerator};
  }

  void set_error(std::exception_ptr e)
  {
    std::scoped_lock guard(error_mutex);
    if (!error)
    {
      error = std::move(e);
    }
    failed.store(true);
  }
};

/// \brief Returns the transitions of the projected state x of the group of the context as an LDD. It is the
/// callback of lddmc_collect (in parallel, on any Lace worker) and of learn_successors_sequential_callback.
/// No exception leaves the callback, which Sylvan calls from C code; the first one is kept in the context.
/// If ActionLabel is true then the multi-action will be rewritten and added to the relation.
template <typename Context, bool ActionLabel>
sylvan::MDD learn_successors_callback(WorkerP* worker, Task*, std::uint32_t* x, std::size_t, void* context)
{
  using enumerator_element = data::enumerator_list_element_with_substitution<>;

  auto& p = *reinterpret_cast<Context*>(context);
  if (p.failed.load(std::memory_order_relaxed))
  {
    return sylvan::lddmc_false;
  }

  auto& algorithm = p.algorithm;
  auto& group = p.group;
  const std::size_t thread_index = p.thread_index(worker);
  auto& data_index = algorithm.data_index();
  std::size_t x_size = group.read.size();
  std::size_t y_size = group.write.size();
  std::size_t xy_size = x_size + y_size;

  if constexpr (ActionLabel)
  {
    // One additional space for the action label.
    xy_size += 1;
  }

  MCRL2_DECLARE_STACK_ARRAY(xy, std::uint32_t, xy_size);

  sylvan::MDD result = sylvan::lddmc_false;
  sylvan::lddmc_refs_pushptr(&result); // a Sylvan garbage collection can happen during union_cube
  try
  {
    // Making the learner (a clone of the rewriter) can throw too, so it is inside the try.
    auto [sigma, rewr, enumerator] = p.resources(worker);
    stopwatch learn_start;

    // add the assignments corresponding to x to sigma, and x to the transition xy
    for (std::size_t j = 0; j < x_size; j++)
    {
      sigma[group.read_parameters[j]] = data_index[group.read[j]].at(x[j], thread_index);
      xy[group.read_pos[j]] = x[j];
    }

    std::size_t i = 0;
    for (const auto& smd: group.summands)
    {
      data::data_expression condition = rewr(smd.condition, sigma);
      if (!data::is_false(condition))
      {
        enumerator.enumerate(enumerator_element(smd.variables, condition),
                             sigma,
                             [&](const enumerator_element& e) {
                               check_enumerator_solution(e, group);
                               e.add_assignments(smd.variables, sigma, rewr);
                               for (std::size_t j = 0; j < y_size; j++)
                               {
                                 data::data_expression value = rewr(smd.next_state[j], sigma);
                                 assert(value != data::undefined_data_expression());

                                 // Determine whether this is a copy parameter, insert special value if that is the case.
                                 xy[group.write_pos[j]] = smd.copy[group.write_pos[j]] ? relprod_ignore : data_index[group.write[j]].insert(value, thread_index).first;
                               }

                               if constexpr (ActionLabel)
                               {
                                 // Action is always located on the last index of the cube.
                                 xy[xy_size - 1] = algorithm.action_index().insert(algorithm.rewrite_action(group.actions[i], rewr, sigma), thread_index).first;
                               }

                               if (!p.parallel)
                               {
                                 // Printing reads the data indices without a lock, which is only safe sequentially.
                                 mCRL2log(log::log_level_t::trace) << "  " << print_transition(data_index, xy.data(), group.read, group.write) << std::endl;
                               }
                               result = p.no_relprod ? sylvan::lddmc_union_cube(result, xy.data(), xy_size)
                                                     : sylvan::lddmc_union_cube_copy(result, xy.data(), const_cast<int*>(smd.copy.data()), xy_size);
                               return false;
                             },
                             data::is_false
        );
      }
      data::remove_assignments(sigma, smd.variables);

      // group.actions has one entry per summand, also for summands whose condition is false.
      ++i;
    }
    data::remove_assignments(sigma, group.read_parameters);
    worker_learn_statistics& statistics = p.statistics[worker->worker];
    statistics.calls += 1;
    statistics.time += learn_start.seconds();
  }
  catch (...)
  {
    p.set_error(std::current_exception());
    result = sylvan::lddmc_false;
  }
  sylvan::lddmc_refs_popptr(1);
  return result;
}

/// \brief The context of learn_successors_sequential_callback: the learn context and the union of the results.
template <typename Context>
struct sequential_learn_context
{
  Context& context;
  sylvan::ldds::ldd result = sylvan::ldds::empty_set();
};

/// \brief A callback for sat_all_nopar that adds the transitions of x, by learn_successors_callback, to the result.
template <typename Context, bool ActionLabel>
void learn_successors_sequential_callback(WorkerP* worker, Task* task, std::uint32_t* x, std::size_t n, void* context)
{
  auto& p = *reinterpret_cast<sequential_learn_context<Context>*>(context);
  // No Sylvan operation happens between the return of the callback and the protection by T.
  sylvan::ldds::ldd T(learn_successors_callback<Context, ActionLabel>(worker, task, x, n, &p.context));
  p.result = sylvan::ldds::union_(p.result, T);
}

/// \brief group.L := group.L U {(x,y) in group | x in X}, with the states of X distributed over the Lace workers if
/// context.parallel holds, and one at a time otherwise.
/// \details It must be called from one thread at a time for the same group: group.L and group.Ldomain are updated
///          on the calling thread, once, after all transitions have been learned.
template <bool ActionLabel, typename Context>
void learn_successors(Context& context, const sylvan::ldds::ldd& X, bool cached)
{
  using namespace sylvan::ldds;
  if (X == empty_set())
  {
    return;
  }

  ldd T;
  if (context.parallel)
  {
    T = collect(X, learn_successors_callback<Context, ActionLabel>, &context);
  }
  else
  {
    sequential_learn_context<Context> sequential{context};
    sat_all_nopar(X, learn_successors_sequential_callback<Context, ActionLabel>, &sequential);
    T = sequential.result;
  }

  if (context.error)
  {
    std::rethrow_exception(context.error);
  }

  auto& group = context.group;
  group.L = union_(group.L, T);
  if (cached)
  {
    group.Ldomain = union_(group.Ldomain, X);
  }

  for (const worker_learn_statistics& s: context.statistics)
  {
    group.learn_time += s.time; // the sum over the workers: CPU time, not wall clock time, in parallel
    group.learn_calls += s.calls;
  }
}

} // namespace mcrl2::symbolic

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_SYMBOLIC_SYMBOLIC_REACHABILITY_H
