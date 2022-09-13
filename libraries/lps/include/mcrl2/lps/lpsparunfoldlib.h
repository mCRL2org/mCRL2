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
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/lps/replace_capture_avoiding.h"

namespace mcrl2::lps
{

struct unfold_cache_element
{
  mcrl2::data::basic_sort cached_fresh_basic_sort;
  mcrl2::data::function_symbol cached_case_function;
  mcrl2::data::function_symbol cached_determine_function;
  mcrl2::data::function_symbol_vector cached_k;
  mcrl2::data::function_symbol_vector cached_projection_functions;

  //mcrl2::data::function_symbol_vector elements_of_new_sorts;
  //mcrl2::data::data_equation_vector data_equations;
};

class lpsparunfold
{
  public:

    // old parameter, case function, determinizing parameter, replacement expressions
    typedef std::vector<std::tuple<data::variable, data::function_symbol, data::variable, data::data_expression_vector>> case_func_vector;

    /** \brief  Constructor for lpsparunfold algorithm.
      * \param[in] spec which is a valid mCRL2 process specification.
      * \param[in,out] cache Cache to store information for reuse.
      * \param[in] add_distribution_laws If true, additional rewrite rules are introduced.
      * \post   The content of mCRL2 process specification analysed for useful information and class variables are set.
      **/
    lpsparunfold(mcrl2::lps::stochastic_specification spec,
        std::map< mcrl2::data::sort_expression , unfold_cache_element > *cache,
        bool add_distribution_laws=false,
        bool alt_case_placement=false
    );


    /** \brief  Destructor for lpsparunfold algorithm.
      **/
    ~lpsparunfold() {};

    /** \brief  Applies lpsparunfold algorithm on a process parameter an mCRL2 process specification .
      * \param[in] parameter_at_index An integer value that represents the index value of an process parameter.
      * \post   The process parameter at index parameter_at_index is unfolded in the mCRL process specification.
      * \return The process specification in which the process parameter at parameter_at_index is unfolded.
    **/
    mcrl2::lps::stochastic_specification algorithm(std::size_t parameter_at_index);

  private:
    /// set of identifiers to use during fresh variable generation
    mcrl2::data::set_identifier_generator m_identifier_generator;

    std::map< mcrl2::data::sort_expression , unfold_cache_element >* m_cache;

    /// \brief The sort of the process parameter that needs to be unfold.
    mcrl2::data::sort_expression m_unfold_process_parameter;

    /// \brief The name of the process parameter that needs to be unfold.
    std::string unfold_parameter_name;

    /// \brief The data_specification used for manipulation
    mcrl2::data::data_specification m_data_specification;

    /// a generator for default data expressions of a give sort;
    mcrl2::data::representative_generator m_representative_generator;

    /// \brief The linear process used for manipulation
    mcrl2::lps::stochastic_linear_process m_lps;

    /// \brief The global variables of the specification
    std::set< mcrl2::data::variable > m_glob_vars;

    /// \brief The initialization of a linear process used for manipulation
    mcrl2::lps::stochastic_process_initializer m_init_process;

    /// \brief The initialization of a linear process
    mcrl2::process::action_label_list m_action_label_list;

    /// \brief The fresh sort of the unfolded process parameter used the case function.
    mcrl2::data::basic_sort fresh_basic_sort;

    /// \brief Mapping of the unfold process parameter to a vector process parameters.
    std::map<mcrl2::data::variable, mcrl2::data::variable_vector > proc_par_to_proc_par_inj;

    /// \brief Boolean to indicate if additional distribution laws need to be generated.
    bool m_add_distribution_laws;

    bool m_alt_case_placement;

    data::data_expression apply_case_function(const data::data_expression& expr, const case_func_vector& case_funcs);
    case_func_vector parameter_case_function(const std::map<data::variable, data::variable_vector >& proc_par_to_proc_par_inj, const data::function_symbol_vector& affected_constructors, const data::function_symbol& case_function);

    /** \brief  Generates a fresh basic sort given an string.
      * \param  str a string value. The value is used to generate a fresh
      *         basic sort.
      * \post   A fresh basic sort is generated.
      * \return A fresh basic sort.
    **/
    mcrl2::data::basic_sort generate_fresh_basic_sort(const std::string& str);

    /** \brief  Generates a fresh name for a constructor or mapping.
      * \param  str a string value. The value is used to generate a fresh
      *         name for a constructor or mapping.
      * \post   A fresh name for a constructor or mapping is generated.
      * \return A fresh name for a constructor or mapping.
    **/
    mcrl2::core::identifier_string generate_fresh_constructor_and_mapping_name(std::string str);

    /** \brief  Creates the case function.
      * \param  k a integer value. The value represents the number of
      *         constructors of the unfolded process parameter.
      * \return A function that returns the corresponding constructor given the
      *         case selector and constructors.
    **/
    mcrl2::data::function_symbol create_case_function(std::size_t k);

    /** \brief  Creates the determine function.
      * \return A function that maps a constructor to the fresh basic sort
    **/
    mcrl2::data::function_symbol create_determine_function();

    /** \brief  Creates projection functions for the unfolded process parameter.
      * \param  k a integer value. The value represents the number of
      *         constructors of the unfolded process parameter.
      * \return A function that returns the projection functions for the
      *         constructor of the unfolded process parameter.
    **/
    mcrl2::data::function_symbol_vector create_projection_functions(mcrl2::data::function_symbol_vector k);

    /** \brief  Creates the needed equations for the unfolded process parameter. The equations are added to m_data_specification.
      * \param  projection_functions a vector with projection functions.
      * \param  case_function the case function.
      * \param  elements_of_new_sorts set of fresh sorts.
      * \param  affected_constructors vector of affected constructors.
      * \param  determine_function the determine function.
      * \return A set of equations for the unfolded process parameter.
    **/
    void create_data_equations(
      const mcrl2::data::function_symbol_vector& projection_functions,
      const mcrl2::data::function_symbol& case_function,
      mcrl2::data::function_symbol_vector elements_of_new_sorts,
      const mcrl2::data::function_symbol_vector& affected_constructors,
      const mcrl2::data::function_symbol& determine_function);

    /** \brief  Determines the constructors that are affected with the unfold
      *         process parameter.
      * \return The constructors that are affected with the unfold
      *         process parameter
    **/
    mcrl2::data::function_symbol_vector determine_affected_constructors();

    /** \brief  Creates a set of constructors for the fresh basic sort
      * \return The constructors that are created for the fresh basic sort
    **/
    mcrl2::data::function_symbol_vector new_constructors(mcrl2::data::function_symbol_vector k);

    /** \brief  Generates a process parameter name given an string.
      * \param str a string value. The value is used to generate a fresh
      *         process parameter name.
      * \param process_parameter_names The context to use for generating fresh names.
      * \post   A fresh process parameter name is generated, which has an unique name
      *         with respect to the set of process parameters (process_parameter_names).
      * \return A fresh process parameter name.
    **/
    mcrl2::core::identifier_string generate_fresh_process_parameter_name(std::string str, std::set<mcrl2::core::identifier_string>& process_parameter_names);

    /** \brief  Get the sort of the process parameter at given index
      * \param  parameter_at_index The index of the parameter for which the sort must be obtained.
      * \return the sort of the process parameter at given index.
    **/
    mcrl2::data::sort_expression sort_at_process_parameter_index(std::size_t parameter_at_index);

    /** \brief  substitute function for replacing process parameters with unfolded process parameters functions.
      * \return substitute function for replacing process parameters with unfolded process parameters functions.
    **/
    std::map<mcrl2::data::variable, mcrl2::data::data_expression> parameter_substitution(
      std::map<mcrl2::data::variable, mcrl2::data::variable_vector > proc_par_to_proc_par_inj,
      mcrl2::data::function_symbol_vector k,
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
      * \param  affected_constructors vector of affected constructors
      * \param  determine_function the determine function
      * \param  parameter_at_index the parameter index
      * \param  pi the projection functions
      * \return a new linear process in which the process parameter at given index is unfolded
    **/
    mcrl2::lps::stochastic_linear_process update_linear_process(
      const mcrl2::data::function_symbol& case_function,
      mcrl2::data::function_symbol_vector affected_constructors,
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
    mcrl2::data::data_equation create_distribution_law_over_case(
      const mcrl2::data::function_symbol& function_for_distribution,
      const mcrl2::data::function_symbol& case_function,
      const bool add_case_function_to_data_type);

    void generate_case_functions(
      mcrl2::data::function_symbol_vector elements_of_new_sorts,
      const mcrl2::data::function_symbol& case_function);

    static bool char_filter(char c)
    {
      // Put unwanted characters here
      return c==' ' || c==':' || c==',' || c=='|'
             || c=='>' || c=='[' || c==']' || c=='@'
             || c=='.' || c=='{' || c=='}' || c=='#'
             || c=='%' || c=='&' || c=='*' || c=='!'
             ;
    }
    /** \brief Add a new equation to m_data_specification.
    **/
    void add_new_equation(const mcrl2::data::data_expression& lhs, const mcrl2::data::data_expression& rhs);

    /** \brief Create a mapping from function symbols to a list of fresh variables that can act as its arguments
    **/
    std::map<mcrl2::data::function_symbol, mcrl2::data::data_expression_vector>
              create_arguments_map(const mcrl2::data::function_symbol_vector& functions);

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
