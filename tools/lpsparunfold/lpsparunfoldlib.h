// Author(s): Frank Stappers 
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsparunfold/lpsparunfoldlib.h
/// \brief Unfold process parameters in mCRL2 process specifications.

#ifndef LPSPARUNFOLDLIB_H
//Fileinfo
#define LPSPARUNFOLDLIB_H

#include <string>
#include <set>
#include <map>

#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/postfix_identifier_generator.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/lps/linear_process.h"
#include <boost/range/iterator_range.hpp>
#include "mcrl2/lps/specification.h"

namespace mcrl2 {
  namespace lps {
    typedef atermpp::vector< mcrl2::lps::summand >     summand_vector;
  }
}

class lpsparunfold 
{
  public:

    /** \brief  Constructor for lpsparunfold algorithm.
      * \param[in] spec which is a valid mCRL2 process specification.
      * \post   The content of mCRL2 process specification analysed for useful information and class variables are set.
      **/
    lpsparunfold( mcrl2::lps::specification spec, bool add_distribution_laws=false);


    /** \brief  Destructor for lpsparunfold algorithm.
      **/
    ~lpsparunfold(){};

    /** \brief  Applies lpsparunfold algorithm on a process parameter an mCRL2 process specification .
      * \param[in] parameter_at_index An integer value that represents the index value of an process parameter.
      * \post   The process parameter at index parameter_at_index is unfolded in the mCRL process specification.
      * \return The process specification in which the process parameter at parameter_at_index is unfolded. 
    **/
    mcrl2::lps::specification algorithm(int parameter_at_index);

  private:
    
    /// \brief The sort of the process parameter that needs to be unfold.
    mcrl2::data::sort_expression m_unfold_process_parameter;

    /// \brief The name of the process parameter that needs to be unfold.
    std::string unfold_parameter_name;

    /// \brief The data_specification used for manipulation 
    mcrl2::data::data_specification m_data_specification;

    /// \brief The linear process used for manipulation 
    mcrl2::lps::linear_process m_lps;

    /// \brief The global variables of the specification
    atermpp::set< mcrl2::data::variable > m_glob_vars;

    /// \brief The initialization of a linear process used for manipulation 
    mcrl2::lps::process_initializer m_init_process;

    /// \brief The initialization of a linear process  
    mcrl2::lps::action_label_list m_action_label_list;

    /// \brief The fresh sort of the unfolded process parameter used the case function.
    mcrl2::data::basic_sort fresh_basic_sort;

    /// \brief The set of sort names occurring in the process specification.
    std::set<mcrl2::core::identifier_string> sort_names;

    /// \brief The set of constructor and mapping names occurring in the process specification.
    std::set<mcrl2::core::identifier_string> mapping_and_constructor_names;

    /// \brief Mapping of the unfold process parameter to a vector process parameters.
    std::map<mcrl2::data::variable, mcrl2::data::variable_vector > proc_par_to_proc_par_inj;

    /// \brief Boolean to indicate if additional distribution laws need to be generated.
    bool m_add_distribution_laws;

    mcrl2::data::function_symbol_vector m_additional_mappings;

    /** \brief  Generates a fresh basic sort given an string.
      * \param  str a string value. The value is used to generate a fresh
      *         basic sort.
      * \post   A fresh basic sort is generated, for which the name is unique
      *         with respect to the set of sort names (sort_names). 
      * \return A fresh basic sort. 
    **/
    mcrl2::data::basic_sort generate_fresh_basic_sort( std::string str );

    /** \brief  Generates a fresh name for a constructor or mapping.
      * \param  str a string value. The value is used to generate a fresh 
      *         name for a constructor or mapping.
      * \post   A fresh name for a constructor or mapping is generated, for 
      *         which the name is unique with respect to the set of mapping 
      *         and constructors  (mapping_and_constructor_names). 
      * \return A fresh name for a constructor or mapping. 
    **/
    mcrl2::core::identifier_string generate_fresh_constructor_and_mapping_name(std::string str);

    /** \brief  Creates the case function.
      * \param  k a integer value. The value represents the number of 
      *         constructors of the unfolded process parameter.
      * \return A function that returns the corresponding constructor given the
      *         case selector and constructors. 
    **/
    mcrl2::data::function_symbol create_case_function(int k);

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
    mcrl2::data::function_symbol_vector create_projection_functions( mcrl2::data::function_symbol_vector k);

    /** \brief  Creates the needed equations for the unfolded process parameter.
      * \param  pi set of projection functions. 
      * \param  case_function the case function.
      * \param  set_of_new_sorts set of fresh sorts.
      * \param  k vector of affected constructors.
      * \param  determine_function the determine function.
      * \return A set of equations for the unfolded process parameter.
    **/
    mcrl2::data::data_equation_vector create_data_equations(
        mcrl2::data::function_symbol_vector pi, 
        mcrl2::data::function_symbol case_function, 
        mcrl2::data::function_symbol_vector set_of_new_sorts,
        mcrl2::data::function_symbol_vector k, 
        mcrl2::data::function_symbol determine_function);

    /** \brief  Determines the constructors that are affected with the unfold
      *         process parameter.
      * \return The constructors that are affected with the unfold
      *         process parameter
    **/
    mcrl2::data::function_symbol_vector determine_affected_constructors();

    /** \brief  Creates a set of constructors for the fresh basic sort 
      * \return The constructors that are created for the fresh basic sort
    **/
    mcrl2::data::function_symbol_vector new_constructors( mcrl2::data::function_symbol_vector k );

    /** \brief  Generates a process parameter name given an string.
      * \param  str a string value. The value is used to generate a fresh
      *         process parameter name.
      * \post   A fresh process parameter name is generated, which has an unique name
      *         with respect to the set of process parameters (process_parameter_names). 
      * \return A fresh process parameter name. 
    **/
    mcrl2::core::identifier_string generate_fresh_process_parameter_name(std::string str, std::set<mcrl2::core::identifier_string>& process_parameter_names );

    /** \brief  Get the sort of the process parameter at given index
      * \param  str denotes the prefered parameter_at_index name for index value.
      * \param  parameters that are already in use
      * \return the sort of the process parameter at given index. 
    **/
    mcrl2::data::sort_expression sort_at_process_parameter_index(int parameter_at_index);

    /** \brief  substitute function for replacing process parameters with unfolded process parameters functions.
      * \return substitute function for replacing process parameters with unfolded process parameters functions. 
    **/
    std::map<mcrl2::data::data_expression, mcrl2::data::data_expression> parameter_substitution(
        std::map<mcrl2::data::variable, mcrl2::data::variable_vector > i, 
        mcrl2::data::function_symbol_vector AffectedConstructors, 
        mcrl2::data::function_symbol case_function );

    /** \brief unfolds a data expression into a vector of process parameters
      * \param  the data expression
      * \param  k vector of affected constructors 
      * \param  determine_function the determine function
      * \param  pi the projection functions 
      * \return The following vector: < det(de), pi_0(de), ... ,pi_n(de) >  
    **/
    mcrl2::data::data_expression_vector unfold_constructor( 
        mcrl2::data::data_expression de, 
        mcrl2::data::function_symbol determine_function, 
        mcrl2::data::function_symbol_vector pi );

    /** \brief substitute unfold process parameter in the linear process
      * \param  case_function the case function
      * \param  k vector of affected constructors 
      * \param  determine_function the determine function
      * \param  parameter_at_index the parameter index
      * \param  pi the projection functions 
      * \return a new linear process in which the process parameter at given index is unfolded
    **/
    mcrl2::lps::linear_process update_linear_process(
        mcrl2::data::function_symbol case_function, 
        mcrl2::data::function_symbol_vector k, 
        mcrl2::data::function_symbol determine_function, 
        int parameter_at_index, 
        mcrl2::data::function_symbol_vector pi);

    /** \brief substitute unfold process parameter in the initialization of the linear process
      * \param  determine_function the determine function
      * \param  parameter_at_index the parameter index
      * \param  pi the projection functions 
      * \return a new initialization for the linear process in which the process parameter at given index is unfolded
    **/
    mcrl2::lps::process_initializer update_linear_process_initialization(
        mcrl2::data::function_symbol determine_function, 
        int parameter_at_index, 
        mcrl2::data::function_symbol_vector pi);

    /** \brief Create distribution rules for distribution_functions over case_functions
    **/
    mcrl2::data::data_equation create_distribution_law_over_case( 
        mcrl2::data::function_symbol distribution_function,
        mcrl2::data::function_symbol case_function); 

    mcrl2::data::data_equation_vector generate_case_functions(
        mcrl2::data::function_symbol_vector elements_of_new_sorts, 
        mcrl2::data::function_symbol case_function );

    static bool char_filter(char c)
    {
        // Put unwanted characters here
	return c==' ' || c==':' || c==',' || c=='|' 
            || c=='>' || c=='[' || c==']' || c=='@' 
            || c=='.' || c=='{' || c=='}' || c=='#'
            || c=='%' || c=='&' || c=='*' || c=='!'
        ;
    }

};


#endif
