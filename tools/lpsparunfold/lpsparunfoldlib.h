#ifndef LPSPARUNFOLDLIB_H
//Fileinfo
#define LPSPARUNFOLDLIB_H

#include <string>
#include <set>
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/postfix_identifier_generator.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/core/identifier_string.h"

#include <boost/range/iterator_range.hpp>

class Sorts 
{
  public:
    //Following line needs the include <boost/range/iterator_range.hpp>
    Sorts( mcrl2::data::data_specification s);
    Sorts( ){ };
    ~Sorts(){};
    void algorithm();
    mcrl2::data::basic_sort unfoldParameter;
  private:
    mcrl2::data::data_specification m_data_specification;
    std::set<mcrl2::data::sort_expression> sortSet;
    std::set<mcrl2::data::function_symbol> consSet;
    std::set<mcrl2::data::function_symbol> mapSet;
    mcrl2::data::basic_sort sort_new;
    mcrl2::data::function_symbol_list affectedConstructors;
    //Functions
    mcrl2::data::basic_sort generateFreshSort( std::string str );
    mcrl2::core::identifier_string generateFreshConMapFuncName(std::string str);
    mcrl2::data::function_symbol createCMap(int k);
    mcrl2::data::function_symbol createDetMap();
    std::set<mcrl2::data::function_symbol> createProjectorFunctions(std::set<mcrl2::data::function_symbol> m);

    std::set<mcrl2::data::function_symbol> determineAffectedConstructors();
    std::set<mcrl2::data::function_symbol> determineAffectedMappings();
    std::set<mcrl2::data::function_symbol> newSorts( std::set<mcrl2::data::function_symbol> k );
    std::set<mcrl2::core::identifier_string> mapping_and_constructor_names;
    std::set<mcrl2::core::identifier_string> sort_names;

    //Functie voor in data lib
    bool basic_sortOccursInSort_expression(mcrl2::data::sort_expression s, mcrl2::data::basic_sort b );

};

/*class Constructors
{
  public:
    Constructors();
    ~Constructors();
  private:
}*/

/*class Mappings
{
  public:
  private:
}

class Variables
{
  public:
  private:
}*/

#endif
