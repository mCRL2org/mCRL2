#include <algorithm>
#include <iostream>
#include <cstdio>
#include <string.h>
#include "lpsparunfoldlib.h"
#include <iostream>
#include <string>
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/print.h"
#include <iterator>

using namespace std;
using namespace mcrl2::core;
using namespace mcrl2::new_data;

Sorts::Sorts(mcrl2::new_data::data_specification s, mcrl2::lps::linear_process lps)
{
  m_data_specification = s;
  m_lps = lps;
  for (data_specification::sorts_const_range::const_iterator i =  s.sorts().begin();
                                                             i != s.sorts().end();
                                                             ++i){
    sortSet.insert(*i);
    if (i->is_basic_sort()) {
      sort_names.insert( (basic_sort(*i)).name() );
    }
   };

  gsVerboseMsg("Specification has %d sorts\n", sortSet.size() );

  {
    // Error: s.constructors( unfoldParameter ); -> 0 constructors 
    data_specification::constructors_const_range fsl= s.constructors();
    for (data_specification::constructors_const_range::const_iterator i = fsl.begin();
                                                                      i != fsl.end();
                                                                      ++i){
      consSet.insert(*i);
      mapping_and_constructor_names.insert( i -> name() );
    };
    gsVerboseMsg("Specification has %d constructors\n", consSet.size() );
  }

  {
    //Error: s.mappings( unfoldParameter ); -> 0 functions
    data_specification::mappings_const_range fsl= s.mappings();
    for (data_specification::mappings_const_range::const_iterator i = fsl.begin();
                                                                  i != fsl.end();
                                                                  ++i){
      mapSet.insert(*i);
    };
    gsVerboseMsg("Specification has %d mappings \n", mapSet.size() );
  }
};


mcrl2::new_data::basic_sort Sorts::generateFreshSort( std::string str )
{
  //Generate a fresh Basic Sort
  mcrl2::new_data::postfix_identifier_generator generator = mcrl2::new_data::postfix_identifier_generator ("");
  generator.add_identifiers( sort_names );
  mcrl2::core::identifier_string nstr = generator( str );
  gsVerboseMsg("Generated a fresh sort: %s\n", string(str).c_str() );
  sort_names.insert(nstr);
  return basic_sort( std::string(nstr) );
}

mcrl2::core::identifier_string Sorts::generateFreshConMapFuncName(std::string str)
{
  //Generate a fresh name for a constructor of mapping
  mcrl2::new_data::postfix_identifier_generator generator = mcrl2::new_data::postfix_identifier_generator ("");
  generator.add_identifiers( mapping_and_constructor_names );
  mcrl2::core::identifier_string nstr = generator( str );
  gsVerboseMsg("Generated a fresh mapping: %s\n", string(nstr).c_str() ); 
  mapping_and_constructor_names.insert( nstr );
  return nstr;
}


function_symbol_vector Sorts::determineAffectedConstructors()
{
  using namespace mcrl2::new_data;

  function_symbol_vector k;
  for( std::set<mcrl2::new_data::function_symbol>::iterator i = consSet.begin();
                                                            i != consSet.end();
                                                            ++i){
    if( i->sort().is_function_sort() ){
      if (function_sort( i->sort()).codomain() == unfoldParameter ){
        k.push_back(*i) ;
      }
    }
    if( i->sort().is_basic_sort() ){
      if (basic_sort( i->sort() ) == unfoldParameter ){
        k.push_back(*i) ;
      }
    }
  }

  gsVerboseMsg("%s has %d constructor function(s)\n", unfoldParameter.name().c_str() , k.size() );

  return k;
}

bool Sorts::basic_sortOccursInSort_expression( mcrl2::new_data::sort_expression s, mcrl2::new_data::basic_sort b )
{
  using namespace mcrl2::new_data;

  if( s.is_basic_sort() )
  {
    if (basic_sort(s) == b)
    {
      return true;
    }
  }
  if( s.is_function_sort() )
  {
    function_sort fs = function_sort(s);
    bool x = basic_sortOccursInSort_expression(fs.codomain(), b ) ;
    boost::iterator_range<sort_expression_list::const_iterator> lst(fs.domain());
    for( sort_expression_list::const_iterator i = lst.begin(); i != lst.end(); ++i ){
      x = x || basic_sortOccursInSort_expression( *i, b );
    }
    return x;
  }
  if( s.is_container_sort() )
  {
    return basic_sortOccursInSort_expression((container_sort(s)).element_sort(), b );
  }
  if( s.is_alias() )
  {
    return basic_sortOccursInSort_expression(alias(s).reference(), b );
  }
  if( s.is_structured_sort() )
  {
    gsVerboseMsg("No structs are yet supported");
    assert("false");
  }

  return false;
}

function_symbol_vector Sorts::determineAffectedMappings()
{
  using namespace mcrl2::new_data;

  function_symbol_vector m;
  for( std::set<mcrl2::new_data::function_symbol>::iterator i = mapSet.begin();
                                                        i != mapSet.end();
                                                         ++i){
    if(basic_sortOccursInSort_expression( i->sort(), unfoldParameter ))
    {
      m.push_back( *i );
      gsDebugMsg("%s: %s\n", i->name().c_str(), i->sort().to_string().c_str()  );
    };
    /* Leuk private functies...
    atermpp::set<sort_expression> x; 
    m_data_specification.dependent_sorts( *i, x  );
    */
  }

  gsVerboseMsg("%s has %d mapping function(s)\n", unfoldParameter.name().c_str() , m.size() );

  return m;
}


function_symbol_vector Sorts::newSorts( mcrl2::new_data::function_symbol_vector k )
{
  using namespace mcrl2::new_data;

  function_symbol_vector set_of_new_sorts;

  for( function_symbol_vector::iterator i = k.begin(); i != k.end(); ++i  )
  {
    std::string prefix = "c_";
    mcrl2::core::identifier_string fresh_name = generateFreshConMapFuncName( prefix.append( i -> name() ) );
    set_of_new_sorts.push_back( function_symbol( fresh_name , sort_new ) );
    gsDebugMsg("%s\n", function_symbol( fresh_name , sort_new ).to_string().c_str() );
    mapping_and_constructor_names.insert(fresh_name);

  }

  gsVerboseMsg("Created %d fresh mapping function(s)\n", set_of_new_sorts.size());
  return set_of_new_sorts;
}

mcrl2::new_data::function_symbol Sorts::createCMap(int k)
{
  mcrl2::new_data::function_symbol fs;
  std::string str = "C_";
  str.append( sort_new.name() );
  mcrl2::core::identifier_string idstr = generateFreshConMapFuncName( str );
  mcrl2::new_data::sort_expression_vector fsl;
  fsl.push_back(sort_new);
  for(int i = 0; i < k; ++i)
  {
    fsl.push_back( unfoldParameter );
  }

  fs = function_symbol( idstr , mcrl2::new_data::function_sort( fsl, sort_new ));
 
  gsVerboseMsg("Created C map");
  gsDebugMsg("%s", fs.to_string().c_str()); 
  gsVerboseMsg("\n");
  gsVerboseMsg("Created C map: %s\n", fs.to_string().c_str());

  return fs;
}

mcrl2::new_data::function_symbol Sorts::createDetMap()
{
  mcrl2::new_data::function_symbol fs;
  std::string str = "Det_";
  str.append( sort_new.name() );
  mcrl2::core::identifier_string idstr = generateFreshConMapFuncName( str );
  mcrl2::new_data::sort_expression_list fsl;
  fs = function_symbol( idstr , mcrl2::new_data::function_sort( unfoldParameter , sort_new ));
  gsVerboseMsg("Created Det map: %s\n", fs.to_string().c_str());

  return fs;
}

mcrl2::new_data::function_symbol_vector Sorts::createProjectorFunctions(function_symbol_vector m)
{
  mcrl2::new_data::function_symbol_vector sfs;
  std::string str = "pi_";
  str.append( sort_new.name() );

  for( function_symbol_vector::iterator i = m.begin() ; i != m.end(); ++i )
  {
    if ( i->sort().is_function_sort() )
    {
      //SEGFAULT: boost::iterator_range<sort_expression_list::const_iterator> sel = function_sort( i->sort() ).domain();
      function_sort fs = function_sort( i->sort() );
      boost::iterator_range<sort_expression_list::const_iterator> sel  = fs.domain();
      for(sort_expression_list::const_iterator j = sel.begin(); j != sel.end(); j++ )
      {
        mcrl2::core::identifier_string idstr = generateFreshConMapFuncName( str );
        //std::cerr << j-> to_string() << std::endl;
         
        sfs.push_back(function_symbol( idstr , mcrl2::new_data::function_sort( unfoldParameter , *j )));

        //Needed for the process parameter extension of an LPS
        affectedSorts.push_back(*j);

      }
    }
  }
  gsVerboseMsg("Created %d projection functions\n", sfs.size() );
  return sfs;
}

std::pair< variable_vector, data_equation_vector > Sorts::createFunctionSection(function_symbol_vector pi, function_symbol Cmap, function_symbol_vector set_of_new_sorts, function_symbol_vector k, function_symbol Detmap)
{
  variable_vector vars;        /* Equation variables  */
  data_equation_vector del;    /* Generated equations */ 
  std::set<mcrl2::core::identifier_string> var_names; /* var_names */
  mcrl2::new_data::postfix_identifier_generator generator = mcrl2::new_data::postfix_identifier_generator ("");
  variable v;
 
  std::string fstr = "y";
  std::string cstr = "c";
  std::string dstr = "d";

  /* Creating variables for projection */
  generator.add_identifiers( var_names );
  mcrl2::core::identifier_string istr = generator( fstr );

  variable lv = variable( istr, unfoldParameter );
  for(int i = 0 ; i < int(pi.size()) ; ++i){
    istr = generator( fstr );
    v = variable( istr, unfoldParameter );
    vars.push_back( v );
  }
  vars.push_back(lv);
    
  /* Creating variable for detector function */
  istr = generator( cstr );
  v = variable( istr, sort_new );
  vars.push_back( v );

  /* Create Equations */ 
  int e = 0; 
  for(function_symbol_vector::iterator i = pi.begin(); i != pi.end(); ++i){
    data_expression_vector args;
    args.push_back(set_of_new_sorts[e]);
    for(int j = 0 ; j < int(pi.size()) ; ++j){
      args.push_back(vars[ j ]);
    }
    //Omslachtig:
    //Zou verwachten  application( data_expression, data_expression_list )
    boost::iterator_range<data_expression_vector::const_iterator> arg (args);
    data_expression lhs = application(  Cmap , arg ); 


    gsDebugMsg("Added equation %s\n", pp(data_equation( lhs, vars[e] )).c_str());
    del.push_back( data_equation( lhs, vars[e] ) );
    ++e;
  }

  {
    data_expression_vector args;
    args.push_back( vars[e+1] );
    for(int j = 0 ; j < int(pi.size()) ; ++j){
        args.push_back(vars[ e ]);
    }
    boost::iterator_range<data_expression_vector::const_iterator> arg (args); /* Omslachtig */
    data_expression lhs = application(  Cmap , arg ); 
    gsDebugMsg("Added equation %s\n", pp(data_equation( lhs, vars[e] )).c_str());
    del.push_back( data_equation( lhs, vars[e] ) );
  }

  /* Creating Detector equations */
  std::map< sort_expression, variable_vector > type_var_list;  /* Function for looking up  Sort |-> [Variable] */
  std::map< sort_expression, int           > type_var_count; /* Function for counting the unique #Sorts of an equation */

  e = 0;
  for(function_symbol_vector::iterator i = k.begin(); i != k.end(); ++i)
  {
    type_var_count.clear();

    if( i -> sort().is_basic_sort() )
    {
      data_expression lhs = application( Detmap, *i );
      gsDebugMsg("Added equation %s\n", pp(data_equation( lhs, set_of_new_sorts[e] )).c_str());
      del.push_back( data_equation( lhs, set_of_new_sorts[e] ) );
    }
    if( i -> sort().is_function_sort() )
    {
      function_sort fs = function_sort( i -> sort() );
      boost::iterator_range<sort_expression_list::const_iterator> sel = fs.domain();

      data_expression_vector dal;
      for(sort_expression_list::const_iterator j = sel.begin(); j != sel.end(); ++j )
      {
         if( j -> is_basic_sort() )
         {
           /* New sort detected */
           if(type_var_list.find( *j ) != type_var_list.end())
           {
             istr = generator( dstr );
             data_expression v = variable( istr, basic_sort( *j ) );
             vars.push_back( v );
             type_var_list[ *j ].push_back( v );
             type_var_count[ *j ] = 1 ;

             dal.push_back( v ); 
           } 
           /* Sort detected that already exists */
           else {
             /* If insufficient variables for sorts are available add a new variable */
             if (type_var_count[ *j ] == int( type_var_list[ *j ].size() ) )
             {
               istr = generator( dstr );
               data_expression v = variable( istr, basic_sort( *j ) );
               vars.push_back( v );
               type_var_list[ *j ].push_back( v );
               type_var_count[ *j ] = ++type_var_count[ *j ] ;
               dal.push_back( v );
             } 
             /* Reuse if variables if there sufficient variables are available */
             else {
               dal.push_back(type_var_list[ *j ].at(type_var_count[ *j ] - 1));
             } 
           }
         } else {
           gsVerboseMsg("Expected only basic sorts in a domain");
           assert(false);
         } 
      }
      boost::iterator_range<data_expression_vector::const_iterator> arg (dal); /* Omslachtig */
      data_expression narg = application( *i , arg );
      data_expression lhs = application( Detmap , narg );
      gsDebugMsg("Added equation %s\n", pp(data_equation( lhs, set_of_new_sorts[e] )).c_str());
      del.push_back( data_equation( lhs, set_of_new_sorts[e] ) );
    
      /* Equations for projection functions */
      int f = 0;
      for(function_symbol_vector::iterator j = pi.begin(); j != pi.end(); ++j){
        data_expression lhs = application( *j,  narg);  
        gsDebugMsg("Added equation %s\n", pp(data_equation( lhs, dal[f] )).c_str());
        del.push_back( data_equation( lhs, dal[f] ) );
        ++f;
      }
    }
  e++;
  }

  std::pair< variable_vector, data_equation_vector> tuple =  std::make_pair( vars , del );

  return tuple;
}

mcrl2::core::identifier_string Sorts::generateFreshProcessParameterName(std::string str)
{
  mcrl2::new_data::postfix_identifier_generator generator = mcrl2::new_data::postfix_identifier_generator ("");
  generator.add_identifiers( process_parameter_names );
  mcrl2::core::identifier_string idstr = generator( str.append( "_pp" ) );
  process_parameter_names.insert( idstr );
  return idstr;
}

void Sorts::updateLPS(function_symbol Cmap , function_symbol_vector AffectedConstructors)
{
   /* Get process parameters from lps */
   mcrl2::new_data::variable_list lps_proc_pars =  m_lps.process_parameters();

   /* Get process_parameters names from lps */
   process_parameter_names.clear();
   std::set<mcrl2::core::identifier_string> process_parameter_names;
   for(mcrl2::new_data::variable_list::iterator i = lps_proc_pars.begin();
                                                i != lps_proc_pars.end();
                                                ++i)
   {
     process_parameter_names.insert(i -> name() );
   }


   /* Create new process parameters */
   mcrl2::new_data::variable_vector new_process_parameters;
   for(mcrl2::new_data::variable_list::iterator i = lps_proc_pars.begin();
                                                i != lps_proc_pars.end();
                                                ++i)
   {
     if( i->sort() == unfoldParameter)
     {
       gsVerboseMsg("unfold parameter %s found at index %d\n", i->name().c_str(), std::distance( lps_proc_pars.begin(), i ) );
       gsDebugMsg("  Inject process parameters\n");
       mcrl2::new_data::variable_vector process_parameters_injection;

       /* Generate fresh process parameter for new Sort */
       mcrl2::core::identifier_string idstr = generateFreshProcessParameterName(unfoldParameter.name());
       process_parameters_injection.push_back( mcrl2::new_data::variable( idstr , sort_new ) );
       gsVerboseMsg("  Created process parameter %s of type %s\n", pp( process_parameters_injection.back() ).c_str(), pp( sort_new ).c_str());

       for( sort_expression_vector::iterator j = affectedSorts.begin()
						; j != affectedSorts.end()
						; ++j )
       {
         mcrl2::core::identifier_string idstr = generateFreshProcessParameterName(unfoldParameter.name());
	 process_parameters_injection.push_back( mcrl2::new_data::variable( idstr , *j ) );
	 gsVerboseMsg("  Created process parameter %s of type %s\n", pp( process_parameters_injection.back() ).c_str(), pp( *j ).c_str());
       }
       new_process_parameters.insert( new_process_parameters.end(), process_parameters_injection.begin(), process_parameters_injection.end() );

       /* store mapping: process parameter -> process parameter injection:
          Required for process parameter replacement in summands
      */
       proc_par_to_proc_par_inj[*i] = process_parameters_injection;

     } else {
       new_process_parameters.push_back( *i );
     }
   }
   gsVerboseMsg("New LPS process parameters: %s\n", mcrl2::new_data::pp(new_process_parameters).c_str() );
   /* Ambiguity
	utility.h std::string mcrl2::new_data::pp(const Container&, typename boost::enable_if<typename mcrl2::new_data::detail::is_container<T>::type, void>::type*) [with Container = mcrl2::new_data::variable_vector]
	print.h:  std::string mcrl2::core::pp(Term, mcrl2::core::t_pp_format) [with Term = atermpp::vector<mcrl2::new_data::variable, std::allocator<mcrl2::new_data::variable> >]
   */

   /*Process summands*/
   mcrl2::lps::summand_list summands = m_lps.summands();
   for( mcrl2::lps::summand_list::iterator i  = summands.begin()
                                         ; i != summands.end()
                                         ; ++i)
   {
     data_expression condition = traverseAndSubtituteDataExpressions( i ->condition(), Cmap, AffectedConstructors );
     gsVerboseMsg("  condition: %s\n", pp(condition).c_str() );
     /* Expected to use multi_action function instead of actions() */
     gsVerboseMsg("  action %s\n", mcrl2::new_data::pp( i -> actions() ).c_str() );
     gsVerboseMsg("  action %s\n", i -> is_tau() ? "true" : "false" );
     
   }

}   

mcrl2::new_data::data_expression Sorts::traverseAndSubtituteDataExpressions( mcrl2::new_data::data_expression de,
                                                                        function_symbol Cmap,
                                                                        function_symbol_vector AffectedConstructors
                                                                      )
{
  if (de.is_application())
  {
    application ap = application(de);
    /* Expected "data_expression_vector" instead of" */
    boost::iterator_range<mcrl2::new_data::detail::term_list_random_iterator<mcrl2::new_data::data_expression> > args = ap.arguments();
    data_expression_vector new_args;
    for(mcrl2::new_data::detail::term_list_random_iterator<mcrl2::new_data::data_expression> i = args.begin();
                                                                                             i != args.end();
                                                                                             ++i
    )
    {
      new_args.push_back( traverseAndSubtituteDataExpressions( *i, Cmap, AffectedConstructors ) );
    }
 
    return 
      application 
        ( ap.head()
        , new_args
        );
  } 
  if (de.is_variable())
  {
    variable var = variable(de);
    if (proc_par_to_proc_par_inj.find( var ) == proc_par_to_proc_par_inj.end() )
    {
      return var;
    } else {
      /* Reconstruct Unfold parameter */
      data_expression_vector new_args ;
      new_args.push_back( proc_par_to_proc_par_inj[ var ][0] );
      for(function_symbol_vector::iterator i =  AffectedConstructors.begin()
                                         ; i != AffectedConstructors.end() 
                                         ; ++i )
      {
        if ( i->sort().is_basic_sort() )
        {
          new_args.push_back( *i );
        }

        if ( i->sort().is_function_sort() )
        {
          data_expression_vector instantiate_domain;
          function_sort::domain_range dcr = function_sort(i->sort()).domain();
          for (function_sort::domain_const_range::iterator j  = dcr.begin()
                                                         ; j != dcr.end() 
                                                         ; j++ )
          {
            for(mcrl2::new_data::variable_vector::iterator k =  proc_par_to_proc_par_inj[ var ].begin() 
                                                         ; k != proc_par_to_proc_par_inj[ var ].end()
                                                         ; ++k )
            {
              if( *j == k->sort() )
              {
                instantiate_domain.push_back( *k );
              } 
            }   
          }
          new_args.push_back( application( *i, instantiate_domain )  );
        }
      }
      data_expression new_expr = application(Cmap, new_args );
      return new_expr;
    }
  } 
  if (de.is_function_symbol())
  {
    return de;
  } 
  if (de.is_abstraction())
  {
    return de;
  } 

  return de;
}

void Sorts::algorithm()
{
   /* Var Dec */
   function_symbol_vector m;
   function_symbol_vector k;
   function_symbol_vector set_of_new_sorts;
   function_symbol_vector set_of_ProjectorFunctions;
   function_symbol Cmap;
   function_symbol Detmap;
//   variable_list data_equation_variables;
//   data_equation_list data_equations;
   std::pair< variable_vector , data_equation_vector > data_specification;
   /*   Alg */
   /*     1 */ sort_new = generateFreshSort( unfoldParameter.name() );
   /*     2 */ k = determineAffectedConstructors();
   /*     3 */ m = determineAffectedMappings();
   /*     4 */ set_of_new_sorts = newSorts( k );
   /*     5 */ sortSet.insert( set_of_new_sorts.begin(), set_of_new_sorts.end() );
               sortSet.insert( sort_new );
   /*     6 */ Cmap = createCMap( k.size() ); 
   /*     7 */ Detmap = createDetMap( );
   /*  8-12 */ set_of_ProjectorFunctions = createProjectorFunctions(k); 
   /* 13-xx */ data_specification = createFunctionSection(set_of_ProjectorFunctions, Cmap, set_of_new_sorts, k, Detmap);

   /*----------------*/

   updateLPS(Cmap, k);  

}
