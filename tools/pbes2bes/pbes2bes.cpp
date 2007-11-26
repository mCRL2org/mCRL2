// Author(s): Alexander van Dam
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2bes.cpp
/// \brief Add your file description here.

#define NAME "pbes2bes"
#define VERSION "July 2007"
#define AUTHOR "Alexander van Dam"


//C++
#include <cstdio>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <sstream>

//Boost
#include <boost/program_options.hpp>

//MCRL-specific
#include "mcrl2/print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"

//LPS-Framework
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/data/data_operators.h"
#include "mcrl2/data/sort_expression.h"

//ATERM-specific
#include "atermpp/substitute.h"
#include "atermpp/utility.h"
#include "atermpp/indexed_set.h"
#include "atermpp/table.h"

//Tool-specific
#include "pbes_rewrite.h"
#include "sort_functions.h"

using namespace std;
using namespace lps;
using namespace mcrl2::utilities;

using atermpp::make_substitution;

namespace po = boost::program_options;

//Type definitions
//----------------
struct t_tool_options {
	string opt_outputformat;
	string opt_strategy;
	string infilename;
	string outfilename;
};

struct t_instantiations {
	data_variable_list finite_var;		// List of all finite variables
	data_variable_list infinite_var;	// List of all infinite variables
	data_expression_list finite_exp;	// List of all finite expressions
	data_expression_list infinite_exp;	// List of all infinite expressions

	void protect()
	{
		finite_var.protect();
		infinite_var.protect();
		finite_exp.protect();
		infinite_exp.protect();
	}
	
	void unprotect()
	{
		finite_var.unprotect();
		infinite_var.unprotect();
		finite_exp.unprotect();
		infinite_exp.unprotect();
	}
	void mark()
	{
		finite_var.mark();
		infinite_var.mark();
		finite_exp.mark();
		infinite_exp.mark();
	}
};


// Specify how the ATerms in t_instantiations need to be protected using a traits class
namespace atermpp
{
	template<>
	struct aterm_traits<t_instantiations>
	{
		static void protect(t_instantiations t) { t.protect(); }
		static void unprotect(t_instantiations t) { t.unprotect(); }
		static void mark(t_instantiations t) { t.mark(); }
	};
} // namespace atermpp

//Functions used by the main program
//----------------------------------
static t_tool_options parse_command_line(int argc, char** argv);
static pbes<> create_bes(pbes<> pbes_spec, t_tool_options tool_options);
pbes<> load_pbes(t_tool_options tool_options);
void save_pbes(pbes<> pbes_spec, t_tool_options tool_options);
void save_pbes_in_cwi_format(pbes<> pbes_spec, string outfilename);
string convert_rhs_to_cwi(pbes_expression p, atermpp::indexed_set *variables);
pbes<> do_lazy_algorithm(pbes<> pbes_spec, t_tool_options tool_options);
pbes<> do_finite_algorithm(pbes<> pbes_spec, t_tool_options tool_options);
propositional_variable_instantiation create_naive_propositional_variable_instantiation(propositional_variable_instantiation propvarinst, atermpp::table *enumerated_sorts);
identifier_string create_propvar_name(identifier_string propvar_name, data_expression_list finite_exp);
atermpp::vector<pbes_equation> sort_names(vector< identifier_string > names_order, atermpp::vector<pbes_equation> to_sort);
ATermAppl FindDummy(ATermAppl sort, lps::pbes<> current_spec, ATermList no_dummy);
//----------------------------------


bool process(t_tool_options const& tool_options) {
  //Load PBES
  pbes<> pbes_spec = load_pbes(tool_options);

  //Process the pbes
  pbes_spec = create_bes(pbes_spec, tool_options);

  //Save PBES
  save_pbes(pbes_spec, tool_options);

  return true;
}

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/squadt_interface.h>

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

    static const char*  pbes_file_for_input;  ///< file containing an LPS
    static const char*  pbes_file_for_output; ///< file used to write the output to

    enum pbes_output_format {
      binary,
      internal,
      cwi
    };

    enum transformation_strategy {
      lazy,
      finite
    };

    static const char* option_transformation_strategy;
    static const char* option_selected_output_format;

  private:

    boost::shared_ptr < tipi::datatype::enumeration > transformation_method_enumeration;
    boost::shared_ptr < tipi::datatype::enumeration > output_format_enumeration;

  public:

    /** \brief constructor */
    squadt_interactor();

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration&);
};

const char* squadt_interactor::pbes_file_for_input  = "pbes_in";
const char* squadt_interactor::pbes_file_for_output = "pbes_out";

const char* squadt_interactor::option_transformation_strategy = "transformation_strategy";
const char* squadt_interactor::option_selected_output_format  = "selected_output_format";

squadt_interactor::squadt_interactor() {
  transformation_method_enumeration.reset(new tipi::datatype::enumeration("lazy"));

  transformation_method_enumeration->add_value("finite");

  output_format_enumeration.reset(new tipi::datatype::enumeration("binary"));

  output_format_enumeration->add_value("internal");
  output_format_enumeration->add_value("cwi");
}

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(pbes_file_for_input, tipi::mime_type("pbes", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  /* Create display */
  tipi::layout::tool_display d;

  // Helper for format selection
  mcrl2::utilities::squadt::radio_button_helper < pbes_output_format > format_selector(d);

  // Helper for strategy selection
  mcrl2::utilities::squadt::radio_button_helper < transformation_strategy > strategy_selector(d);

  layout::vertical_box& m = d.create< vertical_box >();

  m.append(d.create< label >().set_text("Output format : ")).
    append(d.create< horizontal_box >().
                append(format_selector.associate(binary, "binary")).
                append(format_selector.associate(internal, "internal")).
                append(format_selector.associate(cwi, "cwi")),
          margins(0,5,0,5)).
    append(d.create< label >().set_text("Transformation stragey : ")).
    append(strategy_selector.associate(lazy, "lazy: only boolean equations reachable from the initial state")).
    append(strategy_selector.associate(finite, "finite: all possible boolean equations"));

  button& okay_button = d.create< button >().set_label("OK");

  m.append(d.create< label >().set_text(" ")).
    append(okay_button, layout::right);

  /// Copy values from options specified in the configuration
  if (c.option_exists(option_transformation_strategy)) {
    strategy_selector.set_selection(static_cast < transformation_strategy > (
        c.get_option_argument< size_t >(option_transformation_strategy, 0)));
  }
  if (c.option_exists(option_selected_output_format)) {
    format_selector.set_selection(static_cast < pbes_output_format > (
        c.get_option_argument< size_t >(option_selected_output_format, 0)));
  }
  
  send_display_layout(d.set_manager(m));

  /* Wait until the ok button was pressed */
  okay_button.await_change();
  
  /* Add output file to the configuration */
  if (c.output_exists(pbes_file_for_output)) {
    tipi::object& output_file = c.get_output(pbes_file_for_output);
 
    output_file.set_location(c.get_output_name(".pbes"));
  }
  else {
    c.add_output(pbes_file_for_output, tipi::mime_type("pbes", tipi::mime_type::application), c.get_output_name(".pbes"));
  }

  c.add_option(option_transformation_strategy).append_argument(transformation_method_enumeration,
                                static_cast < transformation_strategy > (strategy_selector.get_selection()));
  c.add_option(option_selected_output_format).append_argument(output_format_enumeration,
                                static_cast < pbes_output_format > (format_selector.get_selection()));

  send_clear_display();
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(pbes_file_for_input);
  result &= c.output_exists(pbes_file_for_output);
  result &= c.option_exists(option_transformation_strategy);
  result &= c.option_exists(option_selected_output_format);

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  static std::string strategies[] = { "lazy", "finite" };
  static std::string formats[]    = { "binary", "internal", "cwi" };

  t_tool_options tool_options;

  tool_options.opt_outputformat = formats[c.get_option_argument< size_t >(option_selected_output_format)];
  tool_options.opt_strategy     = strategies[c.get_option_argument< size_t >(option_transformation_strategy)];
  tool_options.infilename       = c.get_input(pbes_file_for_input).get_location();
  tool_options.outfilename      = c.get_output(pbes_file_for_output).get_location();

  bool result = process(tool_options);
 
  send_clear_display();

  return (result);
}
#endif

//Main Program
//------------
/// \brief Main program for pbes2bes
int main(int argc, char** argv)
{
  //Initialise ATerm library and lowlevel-functions
  ATerm bottom;
  ATinit(argc, argv, &bottom);
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (!mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
#endif
    //parse command line
    if (!process(parse_command_line(argc, argv))) {
      return 1;
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}

//function create_bes
//-------------------
/// \brief Instantiate free variables and create the (P)BES with the chosen strategy.
pbes<> create_bes(pbes<> pbes_spec, t_tool_options tool_options)
{
	if (!pbes_spec.is_well_typed())
	{
		gsErrorMsg("The PBES is not well formed. Pbes2bes cannot handle this kind of PBES's\nComputation aborted.\n");
		exit(1);
	}
	if (!pbes_spec.is_closed())
	{
		gsErrorMsg("The PBES is not closed. Pbes2bes cannot handle this kind of PBES's\nComputation aborted.\n");
		exit(1);
	}

	// Instantiate free variables in the system
	atermpp::set< data_variable > freevars = pbes_spec.free_variables();
	if (freevars.size() > 0)
	{
		gsVerboseMsg("Instantiating free variables\n");
	}
	pbes_expression_list replace_freevars;
	for (atermpp::set< data_variable >::iterator i = freevars.begin(); i != freevars.end(); i++)
	{
		pbes_expression freevar_inst = pbes_expression(FindDummy(i->sort(),pbes_spec, ATempty));
		replace_freevars = push_front(replace_freevars, freevar_inst);
	}
	replace_freevars = reverse(replace_freevars);
	
	atermpp::vector<pbes_equation> es = pbes_spec.equations();
	atermpp::vector<pbes_equation> instantiated_es;
	for (atermpp::vector<pbes_equation>::iterator i = es.begin(); i != es.end(); i++)
	{
		pbes_expression expr = i->formula().substitute(make_list_substitution(freevars, replace_freevars));
		instantiated_es.push_back(pbes_equation(i->symbol(), i->variable(), expr));
	}

	pbes_expression instantiated_is_pbe = pbes_expression(pbes_spec.initial_state());
	propositional_variable_instantiation instantiated_is = propositional_variable_instantiation(instantiated_is_pbe.substitute(make_list_substitution(freevars, replace_freevars)));

	pbes_spec = pbes<>(pbes_spec.data(), instantiated_es, instantiated_is);
	
	// Depending on the chosen strategy, create the resulting (P)BES
	if (tool_options.opt_strategy == "finite")
		pbes_spec = do_finite_algorithm(pbes_spec, tool_options);
	else if (tool_options.opt_strategy == "lazy")
		pbes_spec = do_lazy_algorithm(pbes_spec, tool_options);

	//return new pbes
	return pbes_spec;
}

//function FindDummy
//------------------
/// \brief Find a random instantiation of a sort.
ATermAppl FindDummy(ATermAppl sort, pbes<> current_spec, ATermList no_dummy)
{
	ATermList l;

	no_dummy = ATinsert(no_dummy,(ATerm) sort);

	if ( gsIsSortArrow(sort) )
	{
                // Take dataspec from current_spec, then take the consspec from the dataspec
                // and take the list of opids (l) from this consspec
		l = ATLgetArgument(ATAgetArgument(ATAgetArgument(current_spec,0),1),0);
                
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			ATermAppl conssort = ATAgetArgument(ATAgetFirst(l),1);
			if ( ATisEqual(conssort,sort) )
			{
				return ATAgetFirst(l);
			}
		}

                // Take dataspec from current_spec, then take the mapspec from the dataspec
                // and take the list of opids (l) from this mapspec
		l = ATLgetArgument(ATAgetArgument(ATAgetArgument(current_spec,0),2),0);
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			ATermAppl mapsort = ATAgetArgument(ATAgetFirst(l),1);
			if ( ATisEqual(mapsort,sort) )
			{
				return ATAgetFirst(l);
			}
		}
	} else {
		l = ATLgetArgument(ATAgetArgument(ATAgetArgument(current_spec,0),1),0);
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			ATermAppl conssort = ATAgetArgument(ATAgetFirst(l),1);
			if ( ATisEqual(gsGetSortExprResult(conssort),sort) )
			{
				ATermList domains = gsGetSortExprDomains(conssort);
				ATermAppl t = ATAgetFirst(l);
	
				bool found = true;
				for (; !ATisEmpty(domains); domains=ATgetNext(domains))
				{
                                        ATermList domain = ATLgetFirst(domains);
                                        ATermList dummies = ATmakeList0();
                                        for (; !ATisEmpty(domain); domain=ATgetNext(domain))
                                        {
					        if ( ATindexOf(no_dummy,ATgetFirst(domain),0) >= 0 )
					        {
					  	        found = false;
						        break;
					        }
                                                dummies = ATinsert(dummies, (ATerm) FindDummy(ATAgetFirst(domain), current_spec, no_dummy));
                                        }
                                        dummies = ATreverse(dummies);
					t = gsMakeDataAppl(t,dummies);
				}
	
				if ( found )
				{
					return t;
				}
			}
		}
	
		l = ATLgetArgument(ATAgetArgument(ATAgetArgument(current_spec,0),2),0);
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			ATermAppl mapsort = ATAgetArgument(ATAgetFirst(l),1);
			if ( ATisEqual(gsGetSortExprResult(mapsort),sort) )
			{
				ATermList domain = gsGetSortExprDomain(mapsort);
				ATermAppl t = ATAgetFirst(l);
	
				bool found = true;
				for (; !ATisEmpty(domain); domain=ATgetNext(domain))
				{
					if ( ATindexOf(no_dummy,ATgetFirst(domain),0) >= 0 )
					{
						found = false;
						break;
					}
					t = gsMakeDataAppl1(t,FindDummy(ATAgetFirst(domain),current_spec, no_dummy));
				}
	
				if ( found )
				{
					return t;
				}
			}
		}
	}

	gsErrorMsg("could not find dummy of type %T\n",sort);
	exit(1);
}

//function do_lazy_algorithm
//--------------------------
/// \brief Create a BES, using the lazy approach
pbes<> do_lazy_algorithm(pbes<> pbes_spec, t_tool_options tool_options)
{
	gsVerboseMsg("Using lazy approach...\n");
	
	propositional_variable_instantiation initial_state = pbes_spec.initial_state();
	atermpp::vector<pbes_equation> eqsys = pbes_spec.equations();
	data_specification data = pbes_spec.data();

	propositional_variable_instantiation new_initial_state;
	atermpp::vector<pbes_equation> new_equation_system;
	
	atermpp::set< propositional_variable_instantiation > states_todo;
	atermpp::set< propositional_variable_instantiation >::iterator current_state_it;

	int nr_of_equations = 0;
	data_variable_list empty_data_variable_list;
	data_expression_list empty_data_expression_list;

	Rewriter *rewriter = createRewriter(data);

	atermpp::table pbes_equations(2*eqsys.size(), 50); 	// (propvarname, pbes_equation)
	atermpp::indexed_set states_done(10000, 50); 		// (propvarinst)

	vector< identifier_string > names_order;

	gsVerboseMsg("Retrieving PBES equations from equation system...\n");
	for (atermpp::vector<pbes_equation>::iterator eqi = eqsys.begin(); eqi != eqsys.end(); eqi++)
	{
		pbes_equations.put(eqi->variable().name(), *eqi);
		names_order.push_back(eqi->variable().name());
	}

	states_todo.insert(initial_state);

	gsVerboseMsg("Computing BES...\n");
	while (states_todo.size() != 0)
	{
		// Get the first element of the set
		current_state_it = states_todo.begin();
		propositional_variable_instantiation current_state = *current_state_it;
		states_todo.erase(current_state);
		
		// Get equation which belongs to the current propvarinst and their needed parts
		pbes_equation current_pbeq = pbes_equation(pbes_equations.get(current_state.name()));
		propositional_variable current_variable = current_pbeq.variable();
		pbes_expression current_pbes_expression = current_pbeq.formula();

		// Create new propositional variable and propositional variable instantiation
		identifier_string new_propvar_name = create_propvar_name(current_variable.name(), current_state.parameters());
		propositional_variable new_variable = propositional_variable(new_propvar_name, empty_data_variable_list);
		propositional_variable_instantiation new_propvarinst = propositional_variable_instantiation(new_propvar_name, empty_data_expression_list);

		// Add the new instantiation to the table
		states_done.put(new_propvarinst);

		// Replace all occurrences in the right hand side and rewrite the expression
		pbes_expression new_pbes_expression;
		new_pbes_expression = current_pbes_expression.substitute(make_list_substitution(current_variable.parameters(), current_state.parameters()));
		new_pbes_expression = pbes_expression_rewrite(new_pbes_expression, data, rewriter);

		propositional_variable_instantiation_list oldpropvarinst_list;
		propositional_variable_instantiation_list newpropvarinst_list;
		
		// Get all propvarinst of the rhs
		set< propositional_variable_instantiation > propvarinst_set = find_propositional_variable_instantiations(new_pbes_expression);

		for (set< propositional_variable_instantiation >::iterator pvi = propvarinst_set.begin(); pvi != propvarinst_set.end(); pvi++)
		{
			propositional_variable_instantiation temp_pvi = propositional_variable_instantiation(create_propvar_name(pvi->name(), pvi->parameters()), empty_data_expression_list);
			oldpropvarinst_list = push_front(oldpropvarinst_list, *pvi);
			if (states_done.index(temp_pvi) < 0)
			{
				states_todo.insert(*pvi);
			}
			newpropvarinst_list = push_front(newpropvarinst_list, temp_pvi);
		}
		
		// Replace the propvarinsts with the new ones
		new_pbes_expression = new_pbes_expression.substitute(make_list_substitution(oldpropvarinst_list, newpropvarinst_list));

		// Create resulting pbes_equation and add it to equation system 
		new_equation_system.push_back(pbes_equation(current_pbeq.symbol(), new_variable, new_pbes_expression));
		
		if (++nr_of_equations % 1000 == 0)
			gsVerboseMsg("At equation %d\n", nr_of_equations);
	}

	gsVerboseMsg("Sorting result...\n");
	new_equation_system = sort_names(names_order, new_equation_system);

	// Rewrite initial state
	new_initial_state = propositional_variable_instantiation(create_propvar_name(initial_state.name(), initial_state.parameters()), empty_data_expression_list);
	
	pbes<> result = pbes<>(data, new_equation_system, new_initial_state);

	return result;
}

//function sort_names 
//-------------------
/// \brief Sort the equation system with respect to the order of predicate variables in the original equation system
atermpp::vector<pbes_equation> sort_names(vector< identifier_string > names_order, atermpp::vector<pbes_equation> to_sort)
{
	atermpp::vector<pbes_equation> result;
	if (names_order.size() == 1)
	{
		result = to_sort;
	}
	else
	{
		for (vector< identifier_string >::iterator on = names_order.begin(); on != names_order.end(); on++)
		{
			atermpp::vector<pbes_equation> todo;
			for (atermpp::vector<pbes_equation>::iterator eqi = to_sort.begin(); eqi != to_sort.end(); eqi++)
			{
				string name = eqi->variable().name();
				name = name.substr(0, name.find("@"));
				string ons = *on;
				if (name == ons)
				{
					result.push_back(*eqi);
				}
				else
				{
					todo.push_back(*eqi);
				}
			}
			to_sort = todo;
		}
	}

	return result;
}

//function do_finite_algorithm
//---------------------------
/// \brief Create a PBES without finite data sorts, using the finite approach
pbes<> do_finite_algorithm(pbes<> pbes_spec, t_tool_options tool_options)
{
	gsVerboseMsg("Using finite approach...\n");
	
	propositional_variable_instantiation initial_state = pbes_spec.initial_state();
	atermpp::vector<pbes_equation> eqsys = pbes_spec.equations();
	data_specification data = pbes_spec.data();

	atermpp::vector<pbes_equation> result_eqsys;				// resulting equation system
	int nr_of_equations = 0;					// Nr of equations computed
	Rewriter *rewriter = createRewriter(data); 	// Data rewriter

	// Empty data_variable_list and data_expression_list
	data_variable_list empty_data_variable_list;
	data_expression_list empty_data_expression_list;

	atermpp::table sort_enumerations(10,50);

	//Populate sort_enumerations with all enumerations for the finite sorts of the system
	gsVerboseMsg("Enumerating finite data sorts...\n");
	for (atermpp::vector<pbes_equation>::iterator eq_i = eqsys.begin(); eq_i != eqsys.end(); eq_i++)
	{
		data_variable_list parameters = eq_i->variable().parameters();
		for (data_variable_list::iterator p = parameters.begin(); p != parameters.end(); p++)
		{
			sort_expression current_sort = p->sort();
			if (sort_enumerations.get(current_sort) == NULL)
			{ 
				if (check_finite(data.constructors(), current_sort))
				{ 
					data_expression_list enumerations_from_sort = enumerate_constructors(data.constructors(), current_sort);
					sort_enumerations.put(current_sort, enumerations_from_sort);
				}
			}
		}
	}

	gsVerboseMsg("Computing PBES without finite data sorts...\n");
	for (atermpp::vector<pbes_equation>::iterator eq_i = eqsys.begin(); eq_i != eqsys.end(); eq_i++)
	{
		pbes_equation equation = *eq_i;
		
		propositional_variable propvar = equation.variable();
		pbes_expression formula = equation.formula();

		identifier_string propvar_name = propvar.name();
		data_variable_list propvar_parameters = propvar.parameters();


		atermpp::vector< t_instantiations > instantiation_list;
		t_instantiations current_values;		// Current results

		// Add empty instantiation to the list
		instantiation_list.push_back(current_values);

		string propvar_name_string = propvar_name;
		gsVerboseMsg("Creating all possible instantiations for propositional variable %s...\n", propvar_name_string.c_str());
		
		for (data_variable_list::iterator p = propvar_parameters.begin(); p != propvar_parameters.end(); p++)
		{
			atermpp::vector< t_instantiations > intermediate_instantiation_list;
			if (sort_enumerations.get(p->sort()) == NULL)
			{ // The sort is infinite
				for (atermpp::vector< t_instantiations >::iterator inst_i = instantiation_list.begin(); inst_i != instantiation_list.end(); inst_i++)
				{
					current_values = *inst_i;
					current_values.infinite_var = push_back(current_values.infinite_var, *p);
					
					intermediate_instantiation_list.push_back(current_values);
				}
			}
			else
			{ // The sort is finite
				current_values.finite_var = push_back(current_values.finite_var, *p);
				data_expression_list enumerations = sort_enumerations.get(p->sort());

				for (atermpp::vector< t_instantiations >::iterator inst_i = instantiation_list.begin(); inst_i != instantiation_list.end(); inst_i++)
				{
					for (data_expression_list::iterator e = enumerations.begin(); e != enumerations.end(); e++)
					{
						current_values.finite_exp = push_back(inst_i->finite_exp, *e);
						intermediate_instantiation_list.push_back(current_values);
					}
				}
			}
			instantiation_list = intermediate_instantiation_list;
		}
		
		gsVerboseMsg("Computing Boolean equations for each instantiation of propositional variable %s...\n", propvar_name_string.c_str());
		
		for (atermpp::vector< t_instantiations >::iterator inst_i = instantiation_list.begin(); inst_i != instantiation_list.end(); inst_i++)
		{
			propositional_variable propvar_current = propositional_variable(create_propvar_name(propvar_name, inst_i->finite_exp), inst_i->infinite_var);

			pbes_expression current_expression;	// Current expression

			// Substitute all instantiated variables and rewrite the rhs as far as possible.
			current_expression = formula.substitute(make_list_substitution(inst_i->finite_var, inst_i->finite_exp));
			current_expression = pbes_expression_rewrite(current_expression, data, rewriter);
			
			propositional_variable_instantiation_list oldpropvarinst_list;
			propositional_variable_instantiation_list newpropvarinst_list;
			
			// Get all propvarinst of the rhs
			set< propositional_variable_instantiation > propvarinst_set = find_propositional_variable_instantiations(current_expression);

			for (set< propositional_variable_instantiation >::iterator pvi = propvarinst_set.begin(); pvi != propvarinst_set.end(); pvi++)
			{
				oldpropvarinst_list = push_front(oldpropvarinst_list, *pvi);
				propositional_variable_instantiation newpropvarinst = create_naive_propositional_variable_instantiation(*pvi, &sort_enumerations);
				newpropvarinst_list = push_front(newpropvarinst_list, newpropvarinst);
			}

			current_expression = current_expression.substitute(make_list_substitution(oldpropvarinst_list, newpropvarinst_list));

			result_eqsys.push_back(pbes_equation(eq_i->symbol(), propvar_current, current_expression));

			if (++nr_of_equations % 1000 == 0)
				gsVerboseMsg("At Boolean equation %d\n", nr_of_equations);

		}
	}

	gsVerboseMsg("Instantiation process finished.\nNumber of Boolean equations computed: %d\n", nr_of_equations);
	
	// rewrite initial state
	propositional_variable_instantiation new_initial_state = create_naive_propositional_variable_instantiation(initial_state, &sort_enumerations);

	pbes<> result = pbes<>(data, result_eqsys, new_initial_state);

	delete rewriter;

	return result;
}


//function create_naive_propositional_variable_instantiation
//----------------------------------------------------------
/// \brief Create a new propositional variable instantiation with instantiated values and infinite variables
propositional_variable_instantiation create_naive_propositional_variable_instantiation(propositional_variable_instantiation propvarinst, atermpp::table *enumerated_sorts)
{
	data_expression_list finite_expression;
	data_expression_list infinite_expression;
	
	for (data_expression_list::iterator p = propvarinst.parameters().begin(); p != propvarinst.parameters().end(); p++)
	{
		if (enumerated_sorts->get(p->sort()) != NULL)
		{ //sort is finite
			if (is_data_operation(*p))
			{ // If p is a correct data operation
				finite_expression = push_back(finite_expression, *p);
			}
			else if (is_data_variable(*p))
			{ // If p is a freevar
				gsErrorMsg("The propositional varaible contains a variable of finite sort.\n");
				gsErrorMsg("Can not handle variables of finite sort when creating a propositional variable name.\n");
				gsErrorMsg("Computation aborted.\n");
				cout << "Problematic Term: " << pp(*p) << endl;
				exit(1);
			}
		}
		else
		{ //sort is infinite
			infinite_expression = push_back(infinite_expression, *p);
		}
	}

	return propositional_variable_instantiation(create_propvar_name(propvarinst.name(), finite_expression), infinite_expression);
}

//function create_propvar_name
//----------------------------
/// \brief Create a new propositional variable name
identifier_string create_propvar_name(identifier_string propvar_name, data_expression_list del)
{
	string propvar_name_current = propvar_name;
	if (!del.empty())
	{
		for (data_expression_list::iterator del_i = del.begin(); del_i != del.end(); del_i++)
		{
			if (is_data_operation(*del_i))
			{ //If p is a OpId
				propvar_name_current += "@";
				propvar_name_current += pp(*del_i);
			}
			else if (is_data_variable(*del_i))
			{ // If p is a freevar
				gsErrorMsg("The propositional varaible contains a variable of finite sort.\n");
				gsErrorMsg("Can not handle variables of finite sort when creating a propositional variable name.\n");
				gsErrorMsg("Computation aborted.\n");
				cout << "Problematic Term: " << pp(*del_i) << endl;
				exit(1);
			}
			else if (is_data_application(*del_i))
			{ // If p is a data application
				propvar_name_current += "@";
				propvar_name_current += pp(*del_i);
			}
			else
			{
				gsErrorMsg("Can not rewrite the name of the propositional_variable\n");
				cout << "Problematic Term: " << pp(*del_i) << endl;
				exit(1);
			}
			
		}
	}

	return propvar_name_current;
}

//function load_pbes
//------------------
/// \brief Load a PBES.
pbes<> load_pbes(t_tool_options tool_options)
{
	string infilename = tool_options.infilename;

	pbes<> pbes_spec;
	if (infilename == "-")
	{
	  try
	  {
		  pbes_spec.load("-");
		}
    catch (std::runtime_error e)
		{
			gsErrorMsg("Cannot open PBES from stdin\n");
			exit(1);
		}
	}
	else
	{
    try
    {
		  pbes_spec.load(infilename);
		}
    catch (std::runtime_error e)
		{
			gsErrorMsg("Cannot open PBES from '%s'\n", infilename.c_str());
			exit(1);
		}
	}
	return pbes_spec;
}

//function save_pbes
//------------------
/// \brief Save a PBES in the format specified.
void save_pbes(pbes<> pbes_spec, t_tool_options tool_options)
{
	string outfilename = tool_options.outfilename;

	// Check if the result is a BES or a PBES
	bool is_bes = true;
	if ( (!pbes_spec.is_bes()) && (!pbes_spec.initial_state().parameters().empty() ) )
	{
		is_bes = false;
		gsVerboseMsg("The result is a PBES.\n");
	}
	else
	{
		gsVerboseMsg("The result is a BES.\n");
	}
	
	if (tool_options.opt_outputformat == "internal")
	{ // In internal format
		gsVerboseMsg("Saving result in internal format...\n");
		if (!pbes_spec.save(outfilename, false))
		{
			gsErrorMsg("Could not save PBES to %s\n", outfilename.c_str());
			exit(1);
		}
	}
	else if (tool_options.opt_outputformat == "binary")
	{ // in binary format
		gsVerboseMsg("Saving result in binary format...\n");
		if (!pbes_spec.save(outfilename, true))
		{
			gsErrorMsg("Could not save PBES to %s\n", outfilename.c_str());
			exit(1);
		}
	}
	else if (tool_options.opt_outputformat == "cwi")
	{ // in CWI format only if the result is a BES, otherwise Binary
	gsVerboseMsg("Saving result in CWI format...\n");
		if (is_bes)
		{
			save_pbes_in_cwi_format(pbes_spec, outfilename);
		}
		else
		{
			gsWarningMsg("Saving in CWI format not possible. Saving result in binary format.\n");
			if (!pbes_spec.save(outfilename, true))
			{
				gsErrorMsg("Could not save PBES to %s\n", outfilename.c_str());
				exit(1);
			}

		}
	}
}

//function save_pbes_in_cwi_format
//--------------------------------
/// \brief Convert the PBES to the format used at the CWI for their BES solvers
void save_pbes_in_cwi_format(pbes<> pbes_spec, string outfilename)
{
	// Use an indexed set to keep track of the variables and their cwi-representations
	atermpp::vector<pbes_equation> eqsys = pbes_spec.equations();
	atermpp::indexed_set *variables = new atermpp::indexed_set(2*eqsys.size(), 50);
	
	// Fill the indexed set
	for (atermpp::vector<pbes_equation>::iterator eq = eqsys.begin(); eq != eqsys.end(); eq++)
	{
		variables->put(eq->variable());
	} // because pbes is closed, all variables are done at this point

	vector< string > result;
	for (atermpp::vector<pbes_equation>::iterator eq = eqsys.begin(); eq != eqsys.end(); eq++)
	{
		// fixpoint:
		// 	mu => min
		// 	nu => max
		string fp;
		(eq->symbol().is_mu())?fp = "min":fp = "max";

		// variable:
		// 	Integer representation of the propositional variable of the left hand side
	    string variable;
		stringstream variable_stream;
		variable_stream << variables->index(eq->variable());
		variable = variable_stream.str();

		// Convert the right hand side of the equation to the CWI format
		string rhs = convert_rhs_to_cwi(eq->formula(), variables);

		string equation = fp + " " + variable + " = " + rhs + "\n";

		result.push_back(equation);
	}

	if (outfilename != "-")
	{
		ofstream outputfile;
   		outputfile.open(outfilename.c_str(), ios::trunc);
	
		for (vector< string >::iterator res = result.begin(); res != result.end(); res++)
		{
			if (outputfile.is_open())
				outputfile << *res;
			else
			{
				gsErrorMsg("Could not save PBES to %s\n", outfilename.c_str());
				exit(1);
			}
		}
		outputfile.close();
	}
	else
	{
		for (vector< string >::iterator res = result.begin(); res != result.end(); res++)
		{
			cout << *res ;
		}
	}
}

//function convert_rhs_to_cwi
//---------------------------
/// \brief Convert a PBES expression to the format of the right hand side used by the CWI
string convert_rhs_to_cwi(pbes_expression p, atermpp::indexed_set *variables)
{
	string result;
	if (pbes_expr::is_true(p))
		// PBESTrue => T
		result = "T";
	else if (pbes_expr::is_false(p))
		// PBESFalse => F
		result = "F";
	else if (pbes_expr::is_and(p))
	{
		//PBESAnd(a,b) => (a & b)
		string left = convert_rhs_to_cwi(pbes_expr::lhs(p), variables);
		string right = convert_rhs_to_cwi(pbes_expr::rhs(p), variables);
		result = "(" + left + "&" + right + ")";
	}
	else if (pbes_expr::is_or(p))
	{
		//PBESOr(a,b) => (a | b)
		string left = convert_rhs_to_cwi(pbes_expr::lhs(p), variables);
		string right = convert_rhs_to_cwi(pbes_expr::rhs(p), variables);
		result = "(" + left + "|" + right + ")";
	}
	else if (pbes_expr::is_propositional_variable_instantiation(p))
	{
		// PropVar => <Int>
		propositional_variable_instantiation propvarinst = propositional_variable_instantiation(p);
		data_variable_list empty;
		propositional_variable propvar = propositional_variable(propvarinst.name(), empty);
		long variable = variables->index(propvar);
		if (variable < 0)
		{
			gsErrorMsg("Error: The BES is not closed. Write to cwi-format failed.\n");
			exit(1);
		}
		else
		{
			stringstream variable_stream;
			variable_stream << variable;
			result = variable_stream.str();
		}
	}
	else
	{
		gsErrorMsg("The used equation system is not a BES. Could not save this in CWI-format.\n");
		exit(1);
	}
	return result;
}

//function parse_command_line
//---------------------------
/// \brief Parse the command line options.
t_tool_options parse_command_line(int argc, char** argv)
{
	t_tool_options tool_options;
	string opt_outputformat;
	string opt_strategy;
	vector< string > file_names;

	po::options_description desc;

	desc.add_options()
			("strategy,s",	po::value<string>(&opt_strategy)->default_value("lazy"), "use strategy arg (default 'lazy');\n"
							"The following strategies are available:\n"
							"finite  Compute all possible boolean equations\n"
							"lazy    Compute only boolean equations which can be reached from the initial state\n")
			("output,o",	po::value<string>(&opt_outputformat)->default_value("binary"), "use outputformat arg (default 'binary');\n"
			 				"available outputformats are binary, internal and cwi")
			("verbose,v",	"turn on the display of short intermediate messages")
			("debug,d",		"turn on the display of detailed intermediate messages")
			("version",		"display version information")
			("help,h",		"display this help")
			;

	po::options_description hidden("Hidden options");
	hidden.add_options()
			("file_names",	po::value< vector< string > >(), "input/output files")
			;

	po::options_description cmdline_options;
	cmdline_options.add(desc).add(hidden);

	po::options_description visible("Allowed options");
	visible.add(desc);

	po::positional_options_description p;
	p.add("file_names", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		cerr << "Usage: " << argv[0] << " [OPTION]... [INFILE [OUTFILE]]" << endl;
		cerr << "Rewrites PBES from stdin or INFILE to an equivalent BES." << endl;
		cerr << "By default the result is written to stdout, otherwise to OUTFILE." << endl;
		cerr << endl;
		cerr << desc;

		exit(0);
	}

	if (vm.count("version"))
	{
		cerr << NAME << " " << VERSION <<  " (revision " << REVISION << ")" << endl;

		exit(0);
	}

	if (vm.count("debug"))
	{
		gsSetDebugMsg();
	}

	if (vm.count("verbose"))
	{
		gsSetVerboseMsg();
	}

	if (vm.count("output")) // Output format
	{
		opt_outputformat = vm["output"].as< string >();
		if (!((opt_outputformat == "internal") || (opt_outputformat == "binary") || (opt_outputformat == "cwi")))
		{
			gsErrorMsg("Unknown outputformat specified. Available outputformats are binary, internal and cwi\n");
			exit(1);
		}
	}

	if (vm.count("strategy")) // Output format
	{
		opt_strategy = vm["strategy"].as< string >();
		if (!((opt_strategy == "finite") || (opt_strategy == "lazy")))
		{
			gsErrorMsg("Unknown strategy specified. Available strategies are finite and lazy\n");
			exit(1);
		}
	}
	
	if (vm.count("file_names"))
	{
		file_names = vm["file_names"].as< vector< string > >();
	}

	string infilename;
	string outfilename;
	if (file_names.size() == 0)
	{
		// Read from and write to stdin
		infilename = "-";
		outfilename = "-";
	}
	else if ( 2 < file_names.size())
	{
		cerr << NAME << ": Too many arguments" << endl;
		exit(1);
	}
	else
	{
		infilename = file_names[0];
		if (file_names.size() == 2)
		{
			outfilename = file_names[1];
		}
		else
		{
			outfilename = "-";
		}
	}
	
	tool_options.infilename = infilename;
	tool_options.outfilename = outfilename;
	
	tool_options.opt_outputformat = opt_outputformat;
	tool_options.opt_strategy = opt_strategy;
	return tool_options;
}
