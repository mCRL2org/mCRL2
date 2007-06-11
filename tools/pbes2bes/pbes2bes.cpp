// ======================================================================
//
// file          : pbes2bes
// date          : 15-04-2007
// version       : 0.1.3
//
// author(s)     : Alexander van Dam <avandam@damdonk.nl>
//
// ======================================================================


#define NAME "pbes2bes"
#define VERSION "0.1.3"
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
#include "liblowlevel.h"
#include "libprint_c.h"

//LPS-Framework
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/data/data_operators.h"
#include "mcrl2/data/sort.h"

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

//Function declarations used by main program
//------------------------------------------
static t_tool_options parse_command_line(int argc, char** argv);
//Post: The command line options are parsed.
//      The program has aborted with a suitable error code, if:
//		- errors were encounterd
//		- non-standard behaviour was requested (help, version)
//Ret:	The parsed command line options

static pbes create_bes(pbes pbes_spec, t_tool_options tool_options);
//Post: tool_options.infilename contains a PBES ("-" indicates stdin)
//Ret:	The BES generated from the PBES

pbes load_pbes(t_tool_options tool_options);
//Post: tool_options.infilename contains a PBES ("-" indicates stdin)
//Ret: The pbes loaded from infile

void save_pbes(pbes pbes_spec, t_tool_options tool_options);
//Post: pbes_spec is saved
//Ret: -

void save_pbes_in_cwi_format(pbes pbes_spec, string outfilename);
//Post: pbes_spec is saved in cwi-format
//Ret: -

string convert_rhs_to_cwi(pbes_expression p, atermpp::indexed_set *variables);
// Function used to convert a pbes_expression to the variant used by the cwi-output

pbes do_lazy_algorithm(pbes pbes_spec, t_tool_options tool_options);
//

pbes do_finite_algorithm(pbes pbes_spec, t_tool_options tool_options);
//

propositional_variable_instantiation create_naive_propositional_variable_instantiation(propositional_variable_instantiation propvarinst, atermpp::table *enumerated_sorts);
// Create a propositional variable instantiation with the checks needed in the naive algorithm

identifier_string create_propvar_name(identifier_string propvar_name, data_expression_list finite_exp);
// Create a identifier string containing the name for use in the propositional_variable(_instantiation)

equation_system sort_names(vector< identifier_string > names_order, equation_system to_sort);

bool process(t_tool_options const& tool_options) {
  //Load PBES
  pbes pbes_spec = load_pbes(tool_options);

  //Process the pbes
  pbes_spec = create_bes(pbes_spec, tool_options);

  //Save PBES
  save_pbes(pbes_spec, tool_options);

  return true;
}

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <utilities/mcrl2_squadt.h>

class squadt_interactor : public mcrl2_squadt::tool_interface {

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

    boost::shared_ptr < sip::datatype::enumeration > transformation_method_enumeration;
    boost::shared_ptr < sip::datatype::enumeration > output_format_enumeration;

  public:

    /** \brief constructor */
    squadt_interactor();

    /** \brief configures tool capabilities */
    void set_capabilities(sip::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(sip::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(sip::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(sip::configuration&);
};

const char* squadt_interactor::pbes_file_for_input  = "pbes_in";
const char* squadt_interactor::pbes_file_for_output = "pbes_out";

const char* squadt_interactor::option_transformation_strategy = "transformation_strategy";
const char* squadt_interactor::option_selected_output_format  = "selected_output_format";

squadt_interactor::squadt_interactor() {
  transformation_method_enumeration.reset(new sip::datatype::enumeration("lazy"));

  transformation_method_enumeration->add_value("finite");

  output_format_enumeration.reset(new sip::datatype::enumeration("binary"));

  output_format_enumeration->add_value("internal");
  output_format_enumeration->add_value("cwi");
}

void squadt_interactor::set_capabilities(sip::tool::capabilities& c) const {
  c.add_input_combination(pbes_file_for_input, sip::mime_type("pbes", sip::mime_type::application), sip::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(sip::configuration& c) {
  using namespace sip;
  using namespace sip::layout;
  using namespace sip::layout::elements;

  layout::tool_display::sptr display(new layout::tool_display);

  /* Create and add the top layout manager */
  layout::vertical_box::aptr top(new layout::vertical_box);

  /* First column */
  layout::manager* h = new layout::horizontal_box();

  h->add(new label("Output format : "));
  
  squadt_utility::radio_button_helper < pbes_output_format >
        format_selector(h, binary, "binary");

  format_selector.associate(h, internal, "internal");
  format_selector.associate(h, cwi, "cwi");

  if (c.option_exists(option_selected_output_format)) {
    format_selector.set_selection(static_cast < pbes_output_format > (
        c.get_option_argument< size_t >(option_selected_output_format, 0)));
  }
  
  /* Attach row */
  top->add(h, margins(0,5,0,5));

  top->add(new label("Transformation strategy :"));

  squadt_utility::radio_button_helper < transformation_strategy >
        transformation_selector(top, lazy, "lazy: only boolean equations reachable from the initial state");

  transformation_selector.associate(top, finite, "all possible boolean equations");

  if (c.option_exists(option_transformation_strategy)) {
    transformation_selector.set_selection(static_cast < transformation_strategy > (
        c.get_option_argument< size_t >(option_transformation_strategy, 0)));
  }
  
  button* okay_button = new button("OK");

  top->add(okay_button, layout::top);

  display->set_manager(top);

  m_communicator.send_display_layout(display);

  /* Wait until the ok button was pressed */
  okay_button->await_change();

  /* Add output file to the configuration */
  if (c.output_exists(pbes_file_for_output)) {
    sip::object& output_file = c.get_output(pbes_file_for_output);
 
    output_file.set_location(c.get_output_name(".pbes"));
  }
  else {
    c.add_output(pbes_file_for_output, sip::mime_type("pbes", sip::mime_type::application), c.get_output_name(".pbes"));
  }

  c.add_option(option_transformation_strategy).append_argument(transformation_method_enumeration,
                                static_cast < transformation_strategy > (transformation_selector.get_selection()));
  c.add_option(option_selected_output_format).append_argument(output_format_enumeration,
                                static_cast < pbes_output_format > (format_selector.get_selection()));

  send_clear_display();
}

bool squadt_interactor::check_configuration(sip::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(pbes_file_for_input);
  result &= c.output_exists(pbes_file_for_output);
  result &= c.option_exists(option_transformation_strategy);
  result &= c.option_exists(option_selected_output_format);

  return (result);
}

bool squadt_interactor::perform_task(sip::configuration& c) {
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
int main(int argc, char** argv)
{
  //Initialise ATerm library and lowlevel-functions
  ATerm bottom;
  ATinit(argc, argv, &bottom);
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (!mcrl2_squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
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
pbes create_bes(pbes pbes_spec, t_tool_options tool_options)
{
	if (!pbes_spec.is_well_formed())
	{
		gsErrorMsg("The PBES is not well formed. Pbes2bes cannot handle this kind of PBES's\nComputation aborted.\n");
		exit(1);
	}
	if (!pbes_spec.is_closed())
	{
		gsErrorMsg("The PBES is not closed. Pbes2bes cannot handle this kind of PBES's\nComputation aborted.\n");
		exit(1);
	}

	if (tool_options.opt_strategy == "finite")
		pbes_spec = do_finite_algorithm(pbes_spec, tool_options);
	else if (tool_options.opt_strategy == "lazy")
		pbes_spec = do_lazy_algorithm(pbes_spec, tool_options);

	//return new pbes
	return pbes_spec;
}

//function do_lazy_algorithm
//------------------------------
pbes do_lazy_algorithm(pbes pbes_spec, t_tool_options tool_options)
{
	// Verbose msg: doing naive algorithm
	gsVerboseMsg("Computing BES from PBES using lazy algorithm...\n");
	
	// Get all parts from the PBES (data specification, equation system, initial state)
	propositional_variable_instantiation initial_state = pbes_spec.initial_state();
	equation_system eqsys = pbes_spec.equations();
	data_specification data = pbes_spec.data();

	// Variables in which the result is stored
	propositional_variable_instantiation new_initial_state;
	equation_system new_equation_system;
	
	// Variable in which the states which has to be done are stored
	atermpp::set< propositional_variable_instantiation > states_todo;

	// Iterator used for the set of equations which has to be done
	atermpp::set< propositional_variable_instantiation >::iterator current_state_it;

	// Variables used in whole function
	int nr_of_equations = 0;
	data_variable_list empty_data_variable_list;
	data_expression_list empty_data_expression_list;

	// Data rewriter
	Rewriter *rewriter = createRewriter(data);

	// Needed hashtables
	atermpp::table pbes_equations(2*eqsys.size(), 50); 	// (propvarname, pbes_equation)
	atermpp::indexed_set states_done(10000, 50); 		// (propvarinst)

	// Vector with the order of the variable names used for sorting the result
	vector< identifier_string > names_order;

	// Fill the pbes_equations table
	gsVerboseMsg("Retrieving pbes_equations from equation system...\n");
	for (equation_system::iterator eqi = eqsys.begin(); eqi != eqsys.end(); eqi++)
	{
		pbes_equations.put(eqi->variable().name(), *eqi);
		names_order.push_back(eqi->variable().name());
	}

	states_todo.insert(initial_state);

	gsVerboseMsg("Computing BES...\n");
	// As long as there are states to be explored
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

		// Create new variable and variable instantiation
		identifier_string new_propvar_name = create_propvar_name(current_variable.name(), current_state.parameters());
		propositional_variable new_variable = propositional_variable(new_propvar_name, empty_data_variable_list);
		propositional_variable_instantiation new_propvarinst = propositional_variable_instantiation(new_propvar_name, empty_data_expression_list);

		// Add the new instantiation to the table
		states_done.put(new_propvarinst);

		// Replace all occurrences in the right hand side and rewrite the expression
		pbes_expression new_pbes_expression;
		new_pbes_expression = current_pbes_expression.substitute(make_list_substitution(current_variable.parameters(), current_state.parameters()));
		new_pbes_expression = pbes_expression_rewrite(new_pbes_expression, data, rewriter);

		// Lists to replace variables in the rhs
		propositional_variable_instantiation_list oldpropvarinst_list;
		propositional_variable_instantiation_list newpropvarinst_list;
		// Get all propvarinst of the rhs
		set< propositional_variable_instantiation > propvarinst_set = find_propositional_variable_instantiations(new_pbes_expression);

		// For each propvarinst in the set
		for (set< propositional_variable_instantiation >::iterator pvi = propvarinst_set.begin(); pvi != propvarinst_set.end(); pvi++)
		{
			propositional_variable_instantiation temp_pvi = propositional_variable_instantiation(create_propvar_name(pvi->name(), pvi->parameters()), empty_data_expression_list);
			// Put the propvarinst in the old list
			oldpropvarinst_list = push_front(oldpropvarinst_list, *pvi);
			if (states_done.index(temp_pvi) < 0)
			{
				// Add it to the todo list
				states_todo.insert(*pvi);
			}
			newpropvarinst_list = push_front(newpropvarinst_list, temp_pvi);
		}
		
		// Replace the propvarinsts with the new ones
		new_pbes_expression = new_pbes_expression.substitute(make_list_substitution(oldpropvarinst_list, newpropvarinst_list));

		// Create resulting pbes_equation and add it to equation system 
		new_equation_system.push_back(pbes_equation(current_pbeq.symbol(), new_variable, new_pbes_expression));
		// Verbose messages after each 1000 equations
		if (++nr_of_equations % 1000 == 0)
			gsVerboseMsg("At Boolean equation %d\n", nr_of_equations);
	}

	// Sort the new equation system
	gsVerboseMsg("Sorting result...\n");
	new_equation_system = sort_names(names_order, new_equation_system);

	// Rewrite initial state
	new_initial_state = propositional_variable_instantiation(create_propvar_name(initial_state.name(), initial_state.parameters()), empty_data_expression_list);
	
	pbes result = pbes(data, new_equation_system, new_initial_state);

	return result;
}

equation_system sort_names(vector< identifier_string > names_order, equation_system to_sort)
{
	equation_system result;
	if (names_order.size() == 1)
	{
		result = to_sort;
	}
	else
	{
		//equation_system todo;
		for (vector< identifier_string >::iterator on = names_order.begin(); on != names_order.end(); on++)
		{
			equation_system todo;
			for (equation_system::iterator eqi = to_sort.begin(); eqi != to_sort.end(); eqi++)
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
pbes do_finite_algorithm(pbes pbes_spec, t_tool_options tool_options)
{
	// Verbose msg: doing naive algorithm
	gsVerboseMsg("Computing BES from PBES using finite algorithm...\n");
	
	// Get all parts from the PBES (data specification, equation system, initial state)
	propositional_variable_instantiation initial_state = pbes_spec.initial_state();
	equation_system eqsys = pbes_spec.equations();
	data_specification data = pbes_spec.data();

	// Needed variables in the whole function
	equation_system result_eqsys;				// resulting equation system
	int nr_of_equations = 0;					// Nr of equations computed
	Rewriter *rewriter = createRewriter(data); 	// Data rewriter

	// Empty data_variable_list and data_expression_list
	data_variable_list empty_data_variable_list;
	data_expression_list empty_data_expression_list;

	atermpp::table sort_enumerations(10,50);

	//Populate sort_enumerations with all enumerations for finite sorts
	gsVerboseMsg("Enumerating finite data types...\n");
	for (equation_system::iterator eq_i = eqsys.begin(); eq_i != eqsys.end(); eq_i++)
	{
		data_variable_list parameters = eq_i->variable().parameters();
		for (data_variable_list::iterator p = parameters.begin(); p != parameters.end(); p++)
		{
			lps::sort current_sort = p->sort();
			if (sort_enumerations.get(current_sort) == NULL)
			{ // If the sort is not in the enumerated_sorts
				if (check_finite(data.constructors(), current_sort))
				{ // If the sort is finite
					data_expression_list enumerations_from_sort = enumerate_constructors(data.constructors(), current_sort);
					sort_enumerations.put(current_sort, enumerations_from_sort);
				}
			}
		}
	}

	// Create the rewritten equation system
	gsVerboseMsg("Computing BES...\n");
	for (equation_system::iterator eq_i = eqsys.begin(); eq_i != eqsys.end(); eq_i++)
	{
		// Get current equation
		pbes_equation equation = *eq_i;
		// Get all parts from the PBES_EQUATION (propositional_variable, formula)
		propositional_variable propvar = equation.variable();
		pbes_expression formula = equation.formula();

		// Get the name and parameters of the propvar
		identifier_string propvar_name = propvar.name();
		data_variable_list propvar_parameters = propvar.parameters();

		// Verbose print the equation which is currently done
		string propvar_name_string = propvar_name;
		gsVerboseMsg("Rewriting PBES equation with propositional variable %s...\n", propvar_name_string.c_str());

		// Vector of instantiations
		atermpp::vector< t_instantiations > instantiation_list;
		t_instantiations instantiation;			// Needed?
		t_instantiations current_values;		// Current results

		// Add empty instantiation to the list
		instantiation_list.push_back(instantiation);

		// For each variable of the equation
		gsVerboseMsg("Creating all possible instantiations for propositional variable %s...\n", propvar_name_string.c_str());
		for (data_variable_list::iterator p = propvar_parameters.begin(); p != propvar_parameters.end(); p++)
		{
			// Vector of instantiations for intermediate results
			atermpp::vector< t_instantiations > intermediate_instantiation_list;
			if (sort_enumerations.get(p->sort()) == NULL)
			{ // The sort is infinite
				current_values.infinite_var = push_back(current_values.infinite_var, *p);
				for (atermpp::vector< t_instantiations >::iterator inst_i = instantiation_list.begin(); inst_i != instantiation_list.end(); inst_i++)
				{
					// Add current_values to intermediate_instantiation_list
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
		
		// All instantiations for the current equation are computed. Now make equations out of them
		gsVerboseMsg("Computing Boolean equations for each instantiation of propositional variable %s...\n", propvar_name_string.c_str());
		for (atermpp::vector< t_instantiations >::iterator inst_i = instantiation_list.begin(); inst_i != instantiation_list.end(); inst_i++)
		{
			// Create the needed propvar
			propositional_variable propvar_current = propositional_variable(create_propvar_name(propvar_name, inst_i->finite_exp), inst_i->infinite_var);

			pbes_expression current_expression;	// Current expression
			// Substitute all variables which are instantiated

			current_expression = formula.substitute(make_list_substitution(inst_i->finite_var, inst_i->finite_exp));
			// Rewrite the rhs as far as possible
			current_expression = pbes_expression_rewrite(current_expression, data, rewriter);
			// Lists to replace variables in the rhs
			propositional_variable_instantiation_list oldpropvarinst_list;
			propositional_variable_instantiation_list newpropvarinst_list;
			// Get all propvarinst of the rhs
			set< propositional_variable_instantiation > propvarinst_set = find_propositional_variable_instantiations(current_expression);

			// For each propvarinst in the set
			for (set< propositional_variable_instantiation >::iterator pvi = propvarinst_set.begin(); pvi != propvarinst_set.end(); pvi++)
			{
				// Put the propvarinst in the old list
				oldpropvarinst_list = push_front(oldpropvarinst_list, *pvi);
				// Create the new propvarinst
				propositional_variable_instantiation newpropvarinst = create_naive_propositional_variable_instantiation(*pvi, &sort_enumerations);
				// Add the new propvarinst in the new list
				newpropvarinst_list = push_front(newpropvarinst_list, newpropvarinst);
			}

			// Replace the propvarinsts with the new ones
			current_expression = current_expression.substitute(make_list_substitution(oldpropvarinst_list, newpropvarinst_list));

			// Add the new pequation tin the resulting system
			result_eqsys.push_back(pbes_equation(eq_i->symbol(), propvar_current, current_expression));

			// Verbose messages after each 1000 equations
			if (++nr_of_equations % 1000 == 0)
				gsVerboseMsg("At Boolean equation %d\n", nr_of_equations);

		}
	}

	// rewrite initial state
	propositional_variable_instantiation new_initial_state = create_naive_propositional_variable_instantiation(initial_state, &sort_enumerations);

	// create resulting pbes
	pbes result = pbes(data, result_eqsys, new_initial_state);

	gsVerboseMsg("Number of Boolean equation systems computed: %d\n", nr_of_equations);

	delete rewriter;

	return result;
}


//function create_naive_propositional_variable_instantiation
//----------------------------------------------------------
propositional_variable_instantiation create_naive_propositional_variable_instantiation(propositional_variable_instantiation propvarinst, atermpp::table *enumerated_sorts)
{
	data_expression_list finite_expression;
	data_expression_list infinite_expression;
	//For each parameter:
	for (data_expression_list::iterator p = propvarinst.parameters().begin(); p != propvarinst.parameters().end(); p++)
	{
		if (enumerated_sorts->get(p->sort()) != NULL)
		{
			//If it is of finite sort, add it to the list of finite expressions of the propvarinst
			if (is_data_operation(*p))
			{ // If p is a correct data operation
				finite_expression = push_back(finite_expression, *p);
			}
			else if (is_data_variable(*p))
			{ // If p is a freevar
				gsErrorMsg("The PBES contains one or more free variables in a propositional variable instantiation.\n");
				gsErrorMsg("Try using mcrl22lps with the flag -f or --no-freevars to solve this.\n");
				exit(1);
			}
		}
		else
		{
			//If it is of infinite sort, add it to the list of infinite expressions of the propvarinst
			infinite_expression = push_back(infinite_expression, *p);
		}
	}

	return propositional_variable_instantiation(create_propvar_name(propvarinst.name(), finite_expression), infinite_expression);
}

//function create_propvar_name
//----------------------------
identifier_string create_propvar_name(identifier_string propvar_name, data_expression_list del)
{
// string representation of the original propvar_name
	string propvar_name_current = propvar_name;
	// Only add data to the string if the finite_exp list is non-empty. Currently "_" is used as seperator
	if (!del.empty())
	{
		// Add each parameter
		for (data_expression_list::iterator del_i = del.begin(); del_i != del.end(); del_i++)
		{
			if (is_data_operation(*del_i))
			{
				// HIER MAG HET
				propvar_name_current += "@";
				propvar_name_current += pp(*del_i);
			}
			else if (is_data_variable(*del_i))
			{ // If p is a freevar
				gsErrorMsg("The PBES contains one or more free variables in a propositional variable instantiation.\n");
				gsErrorMsg("Try using mcrl22lps with the flag -f or --no-freevars to solve this.\n");
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
pbes load_pbes(t_tool_options tool_options)
{
	string infilename = tool_options.infilename;

	pbes pbes_spec;
	if (infilename == "-")
	{
		if (!pbes_spec.load("-"))
		{
			gsErrorMsg("Cannot open PBES from stdin\n");
			exit(1);
		}
	}
	else
	{
		if (!pbes_spec.load(infilename))
		{
			gsErrorMsg("Cannot open PBES from '%s'\n", infilename.c_str());
			exit(1);
		}
	}
	return pbes_spec;
}

//function save_pbes
//------------------
void save_pbes(pbes pbes_spec, t_tool_options tool_options)
{
	//Outfile string
	string outfilename = tool_options.outfilename;
	gsVerboseMsg("Saving result...\n");

	bool is_bes = true;
	if ( (!pbes_spec.equations().is_bes()) && (!pbes_spec.initial_state().parameters().empty() ) )
	{
		is_bes = false;
		gsWarningMsg("The result is a PBES.\n");
	}
	
	//Write PBES
	if (tool_options.opt_outputformat == "internal")
	{ // In internal format
		if (!pbes_spec.save(outfilename, false))
		{
			gsErrorMsg("Could not save PBES to %s\n", outfilename.c_str());
			exit(1);
		}
	}
	else if (tool_options.opt_outputformat == "binary")
	{ // in binary format
		if (!pbes_spec.save(outfilename, true))
		{
			gsErrorMsg("Could not save PBES to %s\n", outfilename.c_str());
			exit(1);
		}
	}
	else if (tool_options.opt_outputformat == "cwi")
	{ // in CWI format only if the result is a BES, otherwise Binary
		if (is_bes)
		{
			save_pbes_in_cwi_format(pbes_spec, outfilename);
		}
		else
		{
			gsWarningMsg("Saving result in binary format.\n");
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
void save_pbes_in_cwi_format(pbes pbes_spec, string outfilename)
{
	gsVerboseMsg("Converting result to CWI-format...\n");
	// Use an indexed set to keep track of the variables and their cwi-representations
	equation_system eqsys = pbes_spec.equations();
	atermpp::indexed_set *variables = new atermpp::indexed_set(2*eqsys.size(), 50);
	// Fill the indexed set
	for (equation_system::iterator eq = eqsys.begin(); eq != eqsys.end(); eq++)
	{
		variables->put(eq->variable());
	} // because pbes is closed, all variables are done at this point

	vector< string > result;
	// Rewrite all equations to CWI format
	for (equation_system::iterator eq = eqsys.begin(); eq != eqsys.end(); eq++)
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

		// Create the equation
		string equation = fp + " " + variable + " = " + rhs + "\n";

		result.push_back(equation);
	}

	gsVerboseMsg("Saving result...\n");
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
			gsErrorMsg("Unknown outputformat specified. Available outputformats are binary, external and cwi\n");
			exit(1);
		}
	}

	if (vm.count("strategy")) // Output format
	{
		opt_strategy = vm["strategy"].as< string >();
		if (!((opt_strategy == "finite") || (opt_strategy == "lazy")))
		{
			gsErrorMsg("Unknown strategy specified. Available strategies are naive and strategy\n");
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


