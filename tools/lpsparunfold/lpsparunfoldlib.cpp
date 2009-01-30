#include <algorithm>
#include <iostream>
#include <cstdio>
#include <string.h>
#include "lpsparunfoldlib.h"
#include <iostream>
#include <string>
#include "mcrl2/core/messaging.h"


using namespace std;
using namespace mcrl2::core;
using namespace mcrl2::data;

Sorts::Sorts(mcrl2::data::data_specification s)
{
  m_data_specification = s;
  for (sort_expression_list::const_iterator i =  s.sorts().begin(); 
                                            i != s.sorts().end();
                                            ++i){
    sortSet.insert(*i);
  };
  gsVerboseMsg("Specification has %d sorts\n", sortSet.size() );

  {
    // Error: s.constructors( unfoldParameter ); -> 0 constructors 
    function_symbol_list fsl= s.constructors();
    for (function_symbol_list::const_iterator i = fsl.begin();
                                              i != fsl.end();
                                              ++i){
      consSet.insert(*i);
    };
    gsVerboseMsg("Specification has %d constructors\n", consSet.size() );
  }

  {
    //Error: s.functions( unfoldParameter ); -> 0 functions
    boost::iterator_range<function_symbol_list::const_iterator> fsl= s.functions();
    for (function_symbol_list::const_iterator i = fsl.begin();
                                              i != fsl.end();
                                              ++i){
      mapSet.insert(*i);
    };
    gsVerboseMsg("Specification has %d mappings \n", mapSet.size() );
  }
};


mcrl2::data::basic_sort Sorts::generateFreshSort()
{
  //Generate a fresh Basic Sort
  std::set<identifier_string> s;
  for( std::set<mcrl2::data::sort_expression>::iterator i = sortSet.begin();
                                                        i != sortSet.end();
                                                        ++i){
    if (i->is_basic_sort()) {
      s.insert( (basic_sort(*i)).name() );
    }
  };
  mcrl2::data::postfix_identifier_generator generator("");
  generator.add_identifiers( s );
  //TODO -- to_string functie...
  gsVerboseMsg("Generated a fresh sort for %s\n", unfoldParameter.name().c_str() );
  return basic_sort( generator( unfoldParameter.name() ).to_string() );
}

std::set<function_symbol> Sorts::determineAffectedConstructors()
{
  using namespace mcrl2::data;

  std::set<function_symbol> k;
  for( std::set<mcrl2::data::function_symbol>::iterator i = consSet.begin();
                                                        i != consSet.end();
                                                        ++i){
    if( i->sort().is_function_sort() ){
      if (function_sort( i->sort()).codomain() == unfoldParameter ){
        k.insert(*i) ;
      }
    }
    if( i->sort().is_basic_sort() ){
      if (basic_sort( i->sort() ) == unfoldParameter ){
        k.insert(*i) ;
      }
    }
  }

  gsVerboseMsg("%s has %d constructor function(s)\n", unfoldParameter.name().c_str() , k.size() );

  return k;
}

bool Sorts::basic_sortOccursInSort_expression( mcrl2::data::sort_expression s, mcrl2::data::basic_sort b )
{
  using namespace mcrl2::data;

  if( s.is_basic_sort() )
  {
    std::cerr << "b";
    if (basic_sort(s) == b)
    {
      return true;
    }
  }
  if( s.is_function_sort() )
  {
    function_sort sf(s);
    std::cerr << "f" ;
    bool x = basic_sortOccursInSort_expression(sf.codomain(), b ) ;
    boost::iterator_range<sort_expression_list::const_iterator> lst(sf.domain());
    std::cerr << lst.size() ;
    for( sort_expression_list::const_iterator i = lst.begin(); i != lst.end(); ++i ){
      std::cout << i->to_string() << std::endl;
      x = x || basic_sortOccursInSort_expression( *i, b );
    }
    return x;
  }
  if( s.is_container_sort() )
  {
    std::cerr << "c" ;
    return basic_sortOccursInSort_expression((container_sort(s)).element_sort(), b );
  }
  if( s.is_alias() )
  {
    std::cerr << "a" ;
    return basic_sortOccursInSort_expression(alias(s).reference(), b );
  }
  if( s.is_structured_sort() )
  {
    std::cerr << "s" ;
    gsVerboseMsg("No structs are yet supported");
    assert("false");
  }

  return false;
}

std::set<function_symbol> Sorts::determineAffectedMappings()
{
  using namespace mcrl2::data;

  std::set<function_symbol> m;
  for( std::set<mcrl2::data::function_symbol>::iterator i = mapSet.begin();
                                                        i != mapSet.end();
                                                        ++i){
    std::cout << i->to_string() <<std::endl;
    if(basic_sortOccursInSort_expression( i->sort(), unfoldParameter ))
    {
      m.insert( *i );
      std::cout << "--" << i->to_string() <<std::endl;
    };
    std::cerr << std::endl;
    /* Leuk private functies...
    atermpp::set<sort_expression> x; 
    m_data_specification.dependent_sorts( *i, x  );
    */
  }

  gsVerboseMsg("%s has %d mappings function(s)\n", unfoldParameter.name().c_str() , m.size() );

  return m;
}

void Sorts::algorithm()
{
   s_new = generateFreshSort();
   determineAffectedConstructors();
   determineAffectedMappings();
}
