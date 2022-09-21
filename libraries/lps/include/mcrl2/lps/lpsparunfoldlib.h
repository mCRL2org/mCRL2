// Author(s): Frank Stappers
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

struct unfold_cache_element
{
  mcrl2::data::function_symbol_vector affected_constructors;
  mcrl2::data::basic_sort fresh_basic_sort;
  mcrl2::data::function_symbol_vector new_constructors;
  mcrl2::core::identifier_string case_function_name;
  mcrl2::data::function_symbol_vector case_functions;
  mcrl2::data::function_symbol determine_function;
  mcrl2::data::function_symbol_vector projection_functions;

  //mcrl2::data::function_symbol_vector elements_of_new_sorts;
  //mcrl2::data::data_equation_vector data_equations;
};

namespace detail
{
  class data_equation_argument_generator
  {
    public:
      data_equation_argument_generator(
        data::set_identifier_generator& identifier_generator)
        : m_identifier_generator(identifier_generator)
      {}

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
}

class lpsparunfold: public detail::lps_algorithm<lps::stochastic_specification>
{
  typedef typename detail::lps_algorithm<lps::stochastic_specification> super;

  public:

    // old parameter, case function, determinizing parameter, replacement expressions
    typedef std::vector<std::tuple<data::variable, data::function_symbol, data::variable, data::data_expression_vector>> case_func_vector;

    /** \brief  Constructor for lpsparunfold algorithm.
      * \param[in] spec which is a valid mCRL2 process specification.
      * \param[in,out] cache Cache to store information for reuse.
      * \param[in] add_distribution_laws If true, additional rewrite rules are introduced.
      * \post   The content of mCRL2 process specification analysed for useful information and class variables are set.
      **/
    lpsparunfold(lps::stochastic_specification& spec,
                 std::map< data::sort_expression , unfold_cache_element >& cache,
                 bool add_distribution_laws = false, bool alt_case_placement = false);

    /** \brief  Destructor for lpsparunfold algorithm.
      **/
    ~lpsparunfold() {};

    /** \brief  Applies lpsparunfold algorithm on a process parameter of an mCRL2 process specification .
      * \param[in] parameter_at_index An integer value that represents the index value of an process parameter.
      * \post   The process parameter at index parameter_at_index is unfolded in the mCRL2 process specification.
    **/
    void algorithm(std::size_t parameter_at_index);

  private:

    /// set to true when the algorithm has been run once; as the algorithm should
    /// run only once...
    bool m_run_before;

    /// set of identifiers to use during fresh variable generation
    mcrl2::data::set_identifier_generator m_identifier_generator;

    /// generator for arguments in left hand side of data equations
    detail::data_equation_argument_generator m_data_equation_argument_generator;

    /// cache for previously unfolded sorts.
    /// facilitates reuse of previously introduced sorts and function symbols.
    std::map< mcrl2::data::sort_expression , unfold_cache_element >& m_cache;

    /// \brief Cache element for the newly introduced sort.
    /// used in case the unfolded sort is not yet in the cache.
    unfold_cache_element m_new_cache_element;

    /// \brief The process parameter that needs to be unfold.
    mcrl2::data::variable m_unfold_parameter;

    /// a generator for default data expressions of a give sort;
    mcrl2::data::representative_generator m_representative_generator;

    /// \brief Mapping of the unfold process parameter to a vector process parameters.
    std::map<mcrl2::data::variable, mcrl2::data::variable_vector > proc_par_to_proc_par_inj;

    /// \brief Boolean to indicate if additional distribution laws need to be generated.
    bool m_add_distribution_laws;

    bool m_alt_case_placement;

    data::data_expression apply_case_function(const data::data_expression& expr, const case_func_vector& case_funcs);
    case_func_vector parameter_case_function(const std::map<data::variable, data::variable_vector >& proc_par_to_proc_par_inj, const data::function_symbol& case_function);

    /** \brief  Generates a fresh basic sort given an string.
      * \param  str a string value. The value is used to generate a fresh
      *         basic sort.
      * \post   A fresh basic sort is generated.
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

    /** \brief  Generates variable of type sort based on a given string str.
      * \param str a string value. The value is used to generate a fresh
      *         variable name.
      * \param sort The sort of the variable to generate.
      * \post   A fresh variable is generated, which has an unique name.
      * \return A fresh variable.
    **/
    mcrl2::data::variable generate_fresh_variable(std::string str, const data::sort_expression& sort);

    /** \brief  Creates the case function with number of arguments determined by
     *          the number of affected constructors.
     *  \param  sort The sort of the arguments and return sort of the case function
      * \return A function that returns the corresponding constructor given the
      *         case selector and constructors.
    **/
    data::function_symbol create_case_function(const data::sort_expression& sort);

    /** \brief  Creates the determine function.
      * \return A function that maps a constructor to the fresh basic sort
    **/
    void create_determine_function();

    /** \brief  Creates projection functions for the unfolded process parameter.
      * \param  k a integer value. The value represents the number of
      *         constructors of the unfolded process parameter.
      * \return A function that returns the projection functions for the
      *         constructor of the unfolded process parameter.
    **/
    void create_projection_functions();

    /** \brief  Determines the constructors that are affected with the unfold
      *         process parameter.
      * \post The constructors that are affected with the unfold
      *         process parameter are stored in m_affected_constructors
    **/
    void determine_affected_constructors();

    /** \brief  Creates a set of constructors for the fresh basic sort
      * \return The constructors that are created for the fresh basic sort
    **/
    void create_new_constructors();

    /** \brief  Get the process parameter at given index
      * \param  index The index of the parameter which must be obtained.
      * \return the process parameter at given index.
    **/
    mcrl2::data::variable process_parameter_at(const std::size_t index);

    /** \brief  substitute function for replacing process parameters with unfolded process parameters functions.
      * \return substitute function for replacing process parameters with unfolded process parameters functions.
    **/
    std::map<mcrl2::data::variable, mcrl2::data::data_expression> parameter_substitution(
      std::map<mcrl2::data::variable, mcrl2::data::variable_vector > proc_par_to_proc_par_inj,
      const mcrl2::data::function_symbol& case_function);

    /** \brief unfolds a data expression into a vector of process parameters
      * \param  de the data expression
      * \param  determine_function the determine function
      * \param  pi the projection functions
      * \return The following vector: < det(de), pi_0(de), ... ,pi_n(de) >
    **/
    mcrl2::data::data_expression_vector unfold_constructor(
      const mcrl2::data::data_expression& de,
      const mcrl2::data::function_symbol& determine_function,
      mcrl2::data::function_symbol_vector pi);

    /** \brief substitute unfold process parameter in the linear process
      * \param  case_function the case function
      * \param  determine_function the determine function
      * \param  parameter_at_index the parameter index
      * \param  pi the projection functions
      * \return a new linear process in which the process parameter at given index is unfolded
    **/
    mcrl2::lps::stochastic_linear_process update_linear_process(
      const mcrl2::data::function_symbol& case_function,
      const mcrl2::data::function_symbol& determine_function,
      std::size_t parameter_at_index,
      const mcrl2::data::function_symbol_vector& pi);

    /** \brief substitute unfold process parameter in the initialization of the linear process
      * \param  determine_function the determine function
      * \param  parameter_at_index the parameter index
      * \param  pi the projection functions
      * \return a new initialization for the linear process in which the process parameter at given index is unfolded
    **/
    mcrl2::lps::stochastic_process_initializer update_linear_process_initialization(
      const mcrl2::data::function_symbol& determine_function,
      std::size_t parameter_at_index,
      const mcrl2::data::function_symbol_vector& pi);

    /** \brief Create distribution rules for distribution_functions over case_functions
    **/
    void create_distribution_law_over_case(
      const mcrl2::data::function_symbol& function_for_distribution,
      const mcrl2::data::function_symbol& case_function);

    void generate_case_function_equations(
      const mcrl2::data::function_symbol& case_function);

    void generate_determine_function_equations();

    void generate_projection_function_equations();

    static bool char_filter(char c)
    {
      // Put unwanted characters here
      return c==' ' || c==':' || c==',' || c=='|'
             || c=='>' || c=='[' || c==']' || c=='@'
             || c=='.' || c=='{' || c=='}' || c=='#'
             || c=='%' || c=='&' || c=='*' || c=='!'
             ;
    }

    std::string filter_illegal_characters(std::string in) const
    {
      in.resize(std::remove_if(in.begin(), in.end(), &char_filter) - in.begin());
      return in;
    }

    /** \brief Add a new equation to m_data_specification.
    **/
    void add_new_equation(const mcrl2::data::data_expression& lhs, const mcrl2::data::data_expression& rhs);

    // Applies 'process unfolding' to a sequence of summands.
    void unfold_summands(mcrl2::lps::stochastic_action_summand_vector& summands, const mcrl2::data::function_symbol& determine_function, const mcrl2::data::function_symbol_vector& pi);
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
  lpsparunfold::case_func_vector case_funcs;
  data::data_expression current_replacement;

  parunfold_replacement(const lpsparunfold::case_func_vector& case_funcs,
                        data::set_identifier_generator& id_generator)
  : super(sigma1)
  , sigma1(*this, id_generator)
  {
    this->case_funcs = case_funcs;
    if (case_funcs.size() != 1)
    {
      mCRL2log(log::verbose) << "Unfolding more than one parameter somehow" << std::endl;
    }
  }

  template <class T>
  void apply(T& result, const data::application& x)
  {
    if (current_replacement != data::data_expression() || data::is_and(x) || data::is_or(x) || data::is_not(x))
    {
      // if no placement of case functions is underway, or we are still traversing the regular boolean operators, we continue as usual
      super::apply(result, x);
    }
    else
    {
      // place the case functions here
      result = apply_case_function(x);
    }
  }

  data::data_expression apply_case_function(const data::data_expression& expr)
  {
    data::data_expression result = expr;
    for (auto& [par, case_f, det_f, replacements]: case_funcs)
    {
      data::data_expression_vector args;
      args.push_back(det_f);

      for (const data::data_expression& r: replacements)
      {
        current_replacement = r;
        data::data_expression arg;
        super::apply(arg, result);
        args.push_back(arg);
      }
      current_replacement = data::data_expression();

      result = data::application(case_f, args);
    }
    return result;
  }

  // Substitution application
  data::data_expression operator()(const data::variable& x)
  {
    if (current_replacement == data::data_expression())
    {
      return x;
    }
    if (std::get<0>(*case_funcs.begin()) != x)
    {
      throw mcrl2::runtime_error("Unexpected variable at this point");
    }
    return current_replacement;
  }
};

template <template <class> class Builder, template <template <class> class, class, class> class Binder>
parunfold_replacement<Builder, Binder>
apply_parunfold_replacement_builder(const lpsparunfold::case_func_vector& case_funcs,
                                    data::set_identifier_generator& id_generator)
{
  return parunfold_replacement<Builder, Binder>(case_funcs, id_generator);
}

template <typename T>
void insert_case_functions(T& x,
                           const lpsparunfold::case_func_vector& cfv,
                           data::set_identifier_generator& id_generator,
                           typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
)
{
  apply_parunfold_replacement_builder<lps::data_expression_builder, lps::detail::add_capture_avoiding_replacement>(cfv, id_generator).update(x);
}

template <typename T>
void insert_case_functions(T& x,
                           const lpsparunfold::case_func_vector& cfv,
                           typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
)
{
  data::set_identifier_generator id_generator;
  id_generator.add_identifiers(lps::find_identifiers(x));
  for (auto& [par, case_f, det_f, replacements]: cfv)
  {
    id_generator.add_identifier(case_f.name());
    id_generator.add_identifier(det_f.name());
    for (const data::data_expression& r: replacements)
    {
      id_generator.add_identifiers(data::find_identifiers(r));
    }
  }
  insert_case_functions(x, cfv, id_generator);
}

} // namespace mcrl2::lps


#endif
