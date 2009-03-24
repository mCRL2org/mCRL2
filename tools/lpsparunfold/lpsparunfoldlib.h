#ifndef LPSPARUNFOLDLIB_H
//Fileinfo
#define LPSPARUNFOLDLIB_H

#include <string>
#include <set>
#include "mcrl2/new_data/sort_expression.h"
#include "mcrl2/new_data/postfix_identifier_generator.h"
#include "mcrl2/new_data/basic_sort.h"
#include "mcrl2/new_data/data_specification.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/lps/linear_process.h"
#include <boost/range/iterator_range.hpp>

class Sorts 
{
  public:
    //Following line needs the include <boost/range/iterator_range.hpp>
    Sorts( mcrl2::new_data::data_specification s, mcrl2::lps::linear_process lps);
    Sorts( ){ };
    ~Sorts(){};
    void algorithm();
    mcrl2::new_data::basic_sort unfoldParameter;
  private:
    mcrl2::new_data::data_specification m_data_specification;
    mcrl2::lps::linear_process m_lps;
    std::set<mcrl2::new_data::sort_expression> sortSet;
    std::set<mcrl2::new_data::function_symbol> consSet;
    std::set<mcrl2::new_data::function_symbol> mapSet;
    mcrl2::new_data::basic_sort sort_new;
    mcrl2::new_data::function_symbol_vector affectedConstructors;
    //Functions
    mcrl2::new_data::basic_sort generateFreshSort( std::string str );
    mcrl2::core::identifier_string generateFreshConMapFuncName(std::string str);
    mcrl2::new_data::function_symbol createCMap(int k);
    mcrl2::new_data::function_symbol createDetMap();
    mcrl2::new_data::function_symbol_vector createProjectorFunctions( mcrl2::new_data::function_symbol_vector m);
    std::pair< mcrl2::new_data::variable_vector , mcrl2::new_data::data_equation_vector > createFunctionSection(mcrl2::new_data::function_symbol_vector pi, mcrl2::new_data::function_symbol Cmap, mcrl2::new_data::function_symbol_vector set_of_new_sorts,mcrl2::new_data::function_symbol_vector k, mcrl2::new_data::function_symbol Detmap);

    mcrl2::new_data::function_symbol_vector determineAffectedConstructors();
    mcrl2::new_data::function_symbol_vector determineAffectedMappings();
    mcrl2::new_data::function_symbol_vector newSorts( mcrl2::new_data::function_symbol_vector k );
    std::set<mcrl2::core::identifier_string> mapping_and_constructor_names;
    std::set<mcrl2::core::identifier_string> sort_names;

    //Functie voor in data lib
    bool basic_sortOccursInSort_expression(mcrl2::new_data::sort_expression s, mcrl2::new_data::basic_sort b );

};


#endif
