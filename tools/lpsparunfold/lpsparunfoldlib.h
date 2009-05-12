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

namespace mcrl2 {
  namespace lps {
    typedef atermpp::vector< mcrl2::lps::summand >     summand_vector;
  }
}


class Sorts 
{
  public:
    //Following line needs the include <boost/range/iterator_range.hpp>
    Sorts( mcrl2::data::data_specification const& s, mcrl2::lps::linear_process const& lps);
    Sorts( ){ };
    ~Sorts(){};
    void algorithm(int parameter_at_index);
    mcrl2::data::sort_expression unfoldParameter;
  private:
    mcrl2::data::data_specification m_data_specification;
    mcrl2::lps::linear_process m_lps;
    std::set<mcrl2::data::sort_expression> sortSet;
    std::set<mcrl2::data::function_symbol> consSet;
    std::set<mcrl2::data::function_symbol> mapSet;
    mcrl2::data::basic_sort sort_new;
    std::string unfold_parameter_name;
    mcrl2::data::function_symbol_vector affectedConstructors;
    //Functions
    mcrl2::data::basic_sort generateFreshSort( std::string str );
    mcrl2::core::identifier_string generateFreshConMapFuncName(std::string str);
    mcrl2::data::function_symbol createCMap(int k);
    mcrl2::data::function_symbol createDetMap();
    mcrl2::data::function_symbol_vector createProjectorFunctions( mcrl2::data::function_symbol_vector m);
    std::pair< mcrl2::data::variable_vector , mcrl2::data::data_equation_vector > createFunctionSection(mcrl2::data::function_symbol_vector pi, mcrl2::data::function_symbol Cmap, mcrl2::data::function_symbol_vector set_of_new_sorts,mcrl2::data::function_symbol_vector k, mcrl2::data::function_symbol Detmap);

    mcrl2::data::function_symbol_vector determineAffectedConstructors();
//    mcrl2::data::function_symbol_vector determineAffectedMappings( mcrl2::data::function_symbol_vector k );
    mcrl2::data::function_symbol_vector newSorts( mcrl2::data::function_symbol_vector k );
    std::set<mcrl2::core::identifier_string> mapping_and_constructor_names;
    std::set<mcrl2::core::identifier_string> sort_names;

    //Functie voor in data lib
    bool basic_sortOccursInSort_expression(mcrl2::data::sort_expression s, mcrl2::data::basic_sort b );
    mcrl2::core::identifier_string generateFreshProcessParameterName(std::string str);
    std::set<mcrl2::core::identifier_string> process_parameter_names;
    void updateLPS(mcrl2::data::function_symbol Cmap, mcrl2::data::function_symbol_vector, mcrl2::data::function_symbol Detmap, int parameter_at_index, mcrl2::data::function_symbol_vector pi);
  
    //Needed for additional process parameter in the LPS 
    mcrl2::data::sort_expression_vector affectedSorts;
    mcrl2::data::data_expression substituteVariable(mcrl2::data::data_expression var, mcrl2::data::function_symbol Cmap, mcrl2::data::function_symbol_vector AffectedConstructors);
    std::map<mcrl2::data::variable, mcrl2::data::variable_vector > proc_par_to_proc_par_inj;

    std::map<mcrl2::data::data_expression, mcrl2::data::data_expression> parameterSubstitution(std::map<mcrl2::data::variable, mcrl2::data::variable_vector > i, mcrl2::data::function_symbol_vector AffectedConstructors, mcrl2::data::function_symbol Cmap );
    mcrl2::data::data_expression_vector unfoldConstructor( mcrl2::data::data_expression de, mcrl2::data::function_symbol_vector AffectedMappings, mcrl2::data::function_symbol Detmap, mcrl2::data::function_symbol_vector pi );
    mcrl2::data::sort_expression getSortOfProcessParameter(int parameter_at_index);
//    std::map<mcrl2::data::data_expression, mcrl2::data::data_expression> variableSubstitution(std::map<mcrl2::data::variable, mcrl2::data::variable_vector > i, mcrl2::data::function_symbol_vector AffectedConstructors, mcrl2::data::function_symbol Cmap );
//    void deriveConstrutorsFromStructuredSort( mcrl2::data::structured_sort ss );
//    mcrl2::data::structured_sort complete_structured_sort(mcrl2::data::structured_sort s );
};


#endif
