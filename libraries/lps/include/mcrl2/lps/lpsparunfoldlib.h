// Author(s): Frank Stappers, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsparunfold/lpsparunfoldlib.h
/// \brief Unfold process parameters in mCRL2 process specifications.

#ifndef MCRL2_LPS_LPSPARUNFOLDLIB_H
//Fileinfo
#define MCRL2_LPS_LPSPARUNFOLDLIB_H

#include "mcrl2/data/consistency.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/lps/replace_capture_avoiding.h"

namespace mcrl2::lps
{

/// \brief Element in the cache that keeps track of the information for a single
///        unfolded sort, say s.
struct unfold_cache_element
{
  mcrl2::data::function_symbol_vector affected_constructors; // constructors for s
  mcrl2::data::basic_sort fresh_basic_sort; // new sort introduced to represent s
  mcrl2::data::function_symbol_vector new_constructors; // constructors for fresh_basic_sort
  mcrl2::core::identifier_string case_function_name; // name of the case function introduced to deal with sort s
  // case functions for sort s. Note this may be more due to, e.g., adding distribution rules.
  // invariant: case_functions.front() is the case function
  // C: fresh_basic_sort # s # ... # s -> s
  std::map<mcrl2::data::sort_expression, mcrl2::data::function_symbol> case_functions;
  mcrl2::data::function_symbol determine_function;  // Det function for s
  mcrl2::data::function_symbol_vector projection_functions; // pi functions for s
};

namespace detail
{
  /// \brief Fresh variable generator for the arguments of a function symbol.
  ///
  /// Intended for use to generate variables in the left hand side of a data equation.
  /// Therefore, reuse of fresh variables is allowed in multiple equations.
  /// This generator reuses such variable names.
  class data_equation_argument_generator
  {
    public:
      data_equation_argument_generator(
        data::set_identifier_generator& identifier_generator)
        : m_identifier_generator(identifier_generator)
      {}

      /// Generate argument variables for f.
      data::variable_vector arguments(const data::function_symbol& f)
      {
        data::variable_vector result;
        std::map<data::sort_expression, std::size_t> used_arguments;
        if (is_function_sort(f.sort()))
        {
          for(const data::sort_expression& arg_sort: atermpp::down_cast<data::function_sort>(f.sort()).domain())
          {
            // Check if we already used the sort for this function symbol.
            if(used_arguments.find(arg_sort) == used_arguments.end())
            {
              used_arguments[arg_sort] = 0;
            }

            // Check if we need to generate a new variable.
            if(m_variables[arg_sort].size() == used_arguments[arg_sort])
            {
              m_variables[arg_sort].emplace_back(m_identifier_generator("d"), arg_sort);
            }

            // Push new variable
            result.push_back(m_variables[arg_sort].at(used_arguments[arg_sort]));
            ++used_arguments[arg_sort];
          }
        }
        return result;
      }

    protected:
      data::set_identifier_generator& m_identifier_generator;
      std::map<data::sort_expression, data::variable_vector> m_variables;
  };

  class unfold_data_manager
  {
    private:
    /// \brief cache for previously unfolded sorts.
    /// facilitates reuse of previously introduced sorts and function symbols.
    std::map< mcrl2::data::sort_expression , unfold_cache_element >& m_cache;

    data::data_specification& m_dataspec;

    /// \brief set of identifiers to use during fresh variable generation
    mcrl2::data::set_identifier_generator m_identifier_generator;

    /// \brief generator for arguments in left hand side of data equations
    detail::data_equation_argument_generator m_data_equation_argument_generator;

    /// \brief a generator for default data expressions of a given sort;
    mcrl2::data::representative_generator m_representative_generator;

    /// \brief Boolean indicating whether rewrite rules may be added that could make
    ///        the data specification inconsistent.
    bool m_possibly_inconsistent;

    public:
    unfold_data_manager(std::map< mcrl2::data::sort_expression , unfold_cache_element >& cache, data::data_specification& dataspec, bool possibly_inconsistent)
    : m_cache(cache)
    , m_dataspec(dataspec)
    , m_identifier_generator()
    , m_data_equation_argument_generator(m_identifier_generator)
    , m_representative_generator(dataspec)
    , m_possibly_inconsistent(possibly_inconsistent)
    {}

    void add_used_identifier(const core::identifier_string& id)
    {
      m_identifier_generator.add_identifier(id);
    }

    void add_used_identifiers(const std::set<core::identifier_string>& ids)
    {
      m_identifier_generator.add_identifiers(ids);
    }

    data::set_identifier_generator& id_gen()
    {
      return m_identifier_generator;
    }

    unfold_cache_element& get_cache_element(const data::sort_expression& sort);

    /** \brief  Generates a fresh basic sort given a sort expression.
      * \param  sort This sort's name will be used to derive a name for the new sort
      * \return A fresh basic sort.
    **/
    mcrl2::data::basic_sort generate_fresh_basic_sort(const data::sort_expression& sort);

    /** \brief  Generates a fresh name for a constructor or mapping.
      * \param  str a string value. The value is used to generate a fresh
      *         name for a constructor or mapping.
      * \post   A fresh name for a constructor or mapping is generated.
      * \return A fresh name for a constructor or mapping.
    **/
    mcrl2::core::identifier_string generate_fresh_function_symbol_name(const std::string& str);

    /** \brief Generates variable of type sort based on a given string str.
      * \param str a string value. The value is used to generate a fresh
      *         variable name.
      * \param sort The sort of the variable to generate.
      * \post   A fresh variable is generated, which has an unique name.
      * \return A fresh variable.
    **/
    mcrl2::data::variable generate_fresh_variable(std::string str, const data::sort_expression& sort);


    /** \brief  Creates the determine function.
      * \param sort The sort on which this function operates
      * \return A function that maps constructors to the fresh basic sort
    **/
    void create_determine_function(const data::sort_expression& sort);

    /** \brief  Creates projection functions for the unfolded process parameter.
      * \param sort The sort on which these functions operate
      * \return A function that returns the projection functions for the
      *         constructors of the unfolded process parameter.
    **/
    void create_projection_functions(const data::sort_expression& sort);

    /** \brief  Determines the constructors that are affected with the unfold
      *         process parameter.
      * \param sort The sort for which to find constructors
      * \post The constructors that are affected with the unfold
      *         process parameter are stored in m_affected_constructors
    **/
    void determine_affected_constructors(const data::sort_expression& sort);

    /** \brief  Creates a set of constructors for the fresh basic sort
      * \param sort The sort for which to create analogous constructors
      * \return The constructors that are created for the fresh basic sort
    **/
    void create_new_constructors(const data::sort_expression& sort);

    /** \brief  Creates the case function with number of arguments determined by
     *          the number of affected constructors, the sort of the arguments and
     *          result are determined by sort..
     *  \param  det_sort The sort whose constructor is determined in the first argument
     *  \param  output_sort The sort of the arguments and return sort of the case function
      * \return A function that returns the corresponding constructor given the
      *         case selector and constructors.
    **/
    data::function_symbol create_case_function(const data::sort_expression& det_sort, const data::sort_expression& output_sort);

    /** \brief Create distribution rules for distribution_functions over case_functions
    **/
    void create_distribution_law_over_case(
      const data::sort_expression& sort,
      const data::function_symbol& function_for_distribution,
      const data::function_symbol case_function);


    /** \brief Create the data equations for case functions */
    void generate_case_function_equations(
      const data::sort_expression& sort,
      const data::function_symbol& case_function);

    /** \brief Create the data equations for the determine function */
    void generate_determine_function_equations(const data::sort_expression& sort);

    /** \brief Create the data equations for the projection functions */
    void generate_projection_function_equations(const data::sort_expression& sort);

    static bool char_filter(char c)
    {
      // Put unwanted characters here
      return c==' ' || c==':' || c==',' || c=='|'
             || c=='>' || c=='[' || c==']' || c=='@'
             || c=='.' || c=='{' || c=='}' || c=='#'
             || c=='%' || c=='&' || c=='*' || c=='!'
             || c=='(' || c==')'
             ;
    }

    std::string filter_illegal_characters(std::string in) const
    {
      in.resize(std::remove_if(in.begin(), in.end(), &char_filter) - in.begin());
      return in;
    }
  };
}

class lpsparunfold: public detail::lps_algorithm<lps::stochastic_specification>
{
  typedef typename detail::lps_algorithm<lps::stochastic_specification> super;

  public:

    // old parameter, case function, determinizing parameter, replacement expressions
    typedef std::tuple<data::variable, std::map<data::sort_expression, data::function_symbol>, data::variable, data::data_expression_vector> case_func_replacement;

    /** \brief  Constructor for lpsparunfold algorithm.
      * \param[in] spec which is a valid mCRL2 process specification.
      * \param[in,out] cache Cache to store information for reuse.
      * \param[in] alt_case_placement If true, case functions are placed at a higher level.
      * \param[in] possibly_inconsistent If true, case functions over Booleans are replaced by a disjunction of conjunctions.
      *                 For this to be correct, the unfolded sort needs to satisfy some restrictions.
      * \post   The content of mCRL2 process specification analysed for useful information and class variables are set.
      **/
    lpsparunfold(lps::stochastic_specification& spec,
                 std::map< data::sort_expression , unfold_cache_element >& cache,
                 bool alt_case_placement = false,
                 bool possibly_inconsistent = false);

    /** \brief  Applies lpsparunfold algorithm on a process parameter of an mCRL2 process specification .
     *  \pre algorithm has not been called before.
     *  \param[in] parameter_at_index An integer value that represents the index value of an process parameter.
     *  \post   The process parameter at index parameter_at_index is unfolded in the mCRL2 process specification.
    **/
    void algorithm(const std::size_t parameter_at_index);

  private:
    /// \brief set to true when the algorithm has been run once; as the algorithm should
    /// run only once...
    bool m_run_before;

    /// @brief Bookkeeper for recogniser and projection functions.
    detail::unfold_data_manager m_datamgr;

    /// \brief The process parameter that needs to be unfold.
    mcrl2::data::variable m_unfold_parameter;

    /// \brief The process parameters that are inserted.
    mcrl2::data::variable_vector m_injected_parameters;

    /// \brief Boolean to indicate if alternative placement of case functions should be used.
    bool m_alt_case_placement;


    //data::data_expression apply_case_function(const data::data_expression& expr, const case_func_replacement& case_funcs);
    case_func_replacement parameter_case_function();

    /** \brief  Get the process parameter at given index
      * \param  index The index of the parameter which must be obtained.
      * \return the process parameter at given index.
    **/
    mcrl2::data::variable process_parameter_at(const std::size_t index);

    /** \brief  substitute function for replacing process parameters with unfolded process parameters functions.
      * \return substitute function for replacing process parameters with unfolded process parameters functions.
    **/
    std::map<mcrl2::data::variable, mcrl2::data::data_expression> parameter_substitution();

    data::data_expression apply_function(const data::function_symbol& f, const data::data_expression& de) const;

    /** \brief unfolds a data expression into a vector of process parameters
      * \param  de the data expression
      * \return The following vector: < det(de), pi_0(de), ... ,pi_n(de) >
    **/
    mcrl2::data::data_expression_vector unfold_constructor(
      const mcrl2::data::data_expression& de);

    /** \brief substitute unfold process parameter in the linear process
      * \param  parameter_at_index the parameter index
      * \post the process parameter at given index is unfolded in the the linear process
    **/
    void update_linear_process(std::size_t parameter_at_index);

    /** \brief substitute unfold process parameter in the initialization of the linear process
      * \param  parameter_at_index the parameter index
      * \post the initialization for the linear process is updated by unfolding the parameter at given index is unfolded
    **/
   void update_linear_process_initialization(
      std::size_t parameter_at_index);

    // Applies 'process unfolding' to a sequence of summands.
    void unfold_summands(mcrl2::lps::stochastic_action_summand_vector& summands);
};


template <template <class> class Builder, template <template <class> class, class, class> class Binder>
struct parunfold_replacement: public
Binder<Builder, parunfold_replacement<Builder, Binder>, parunfold_replacement<Builder, Binder>>
{
  typedef Binder<Builder, parunfold_replacement<Builder, Binder>, parunfold_replacement<Builder, Binder>> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  data::detail::capture_avoiding_substitution_updater<parunfold_replacement<Builder, Binder>> sigma1;
  lpsparunfold::case_func_replacement case_funcs;
  data::data_expression current_replacement;

  parunfold_replacement(const lpsparunfold::case_func_replacement& case_funcs,
                        data::set_identifier_generator& id_generator)
  : super(sigma1)
  , sigma1(*this, id_generator)
  {
    this->case_funcs = case_funcs;
  }

  template <class T>
  void apply(T& result, const data::application& x)
  {
    if (current_replacement != data::data_expression() || data::is_and(x) || data::is_or(x) || data::is_not(x) || data::is_imp(x) || data::is_if_application(x))
    {
      // if no placement of case functions is underway, or we are still traversing the regular boolean operators, we continue as usual
      super::apply(result, x);
    }
    else
    {
      // place the case functions here
      apply_case_function(result, x);
    }
  }

  void apply_case_function(data::data_expression& result, const data::application& expr)
  {
    auto& [par, case_f, det_f, replacements] = case_funcs;

    if (data::find_free_variables(expr).count(par) == 0)
    {
      // variable to be replaced does not occur here
      // make sure to still apply the substitutions necessary for the capture avoiding tricks
      // NB: stack overflow happens if type of second argument is 'data::data_expression'.
      super::apply(result, expr);
    }
    else
    {
      data::data_expression_vector args;
      args.push_back(det_f);

      for (const data::data_expression& r : replacements)
      {
        current_replacement = r;
        data::data_expression arg;
        super::apply(arg, expr);
        args.push_back(arg);
      }
      current_replacement = data::data_expression();

      if (case_f.find(expr.sort()) == case_f.end())
      {
        throw mcrl2::runtime_error("Case function with target sort " +
                                   data::pp(expr.sort()) + " not declared.");
      }
      result = data::application(case_f[expr.sort()], args);
    }
  }

  // Substitution application
  data::data_expression operator()(const data::variable& x)
  {
    if (current_replacement == data::data_expression())
    {
      return x;
    }
    if (std::get<0>(case_funcs) != x)
    {
      return x;
    }
    return current_replacement;
  }
};

template <template <class> class Builder, template <template <class> class, class, class> class Binder>
parunfold_replacement<Builder, Binder>
apply_parunfold_replacement_builder(const lpsparunfold::case_func_replacement& case_funcs,
                                    data::set_identifier_generator& id_generator)
{
  return parunfold_replacement<Builder, Binder>(case_funcs, id_generator);
}

template <typename T>
void insert_case_functions(T& x,
                           const lpsparunfold::case_func_replacement& cfv,
                           data::set_identifier_generator& id_generator,
                           typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
)
{
  apply_parunfold_replacement_builder<lps::data_expression_builder, lps::detail::add_capture_avoiding_replacement>(cfv, id_generator).update(x);
}

template <typename T>
void insert_case_functions(T& x,
                           const lpsparunfold::case_func_replacement& cfv,
                           typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
)
{
  data::set_identifier_generator id_generator;
  id_generator.add_identifiers(lps::find_identifiers(x));
  auto& [par, case_f, det_f, replacements] = cfv;

  // assumes all case functions have the same name and that par.sort() is in the map of case functions
  id_generator.add_identifier(case_f.find(par.sort())->second.name());
  id_generator.add_identifier(det_f.name());
  for (const data::data_expression& r: replacements)
  {
    id_generator.add_identifiers(data::find_identifiers(r));
  }

  insert_case_functions(x, cfv, id_generator);
}

} // namespace mcrl2::lps


#endif
