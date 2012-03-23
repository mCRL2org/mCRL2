// Author(s): Gijs Kant
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_explorer.h
/// \brief
#ifndef PBES_EXPLORER_H
#define PBES_EXPLORER_H

#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/detail/ppg_traverser.h"
#include "mcrl2/pbes/detail/ppg_rewriter.h"
#include "mcrl2/pbes/parity_game_generator.h"
#include "mcrl2/pbes/detail/pbes_greybox_interface.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::detail;
using namespace mcrl2::core;
using namespace mcrl2::data;

namespace mcrl2
{

namespace pbes_system
{




/// \brief
class lts_type {
	int state_length;
  std::vector<std::string> state_names;
  std::vector<std::string> state_types;
  std::vector<std::string> state_type_list;
  std::map<std::string,int> state_type_index;
  std::vector<int> state_type_no;
  std::vector<std::string> state_label_names;
  std::vector<std::string> state_label_types;
  std::vector<std::string> edge_label_names;
  std::vector<std::string> edge_label_types;

public:
	/// \brief Contructor.
	/// \param state_length
  lts_type(int state_length);

	/// \brief Destructor.
	~lts_type();

	/// \brief Returns the state length.
	int get_state_length() const;

	/// \brief Returns the number of state types.
	int get_number_of_state_types() const;

	/// \brief Returns the sequence of state part names.
	const std::vector<std::string>& get_state_names() const;

	/// \brief Returns the sequence of state part types.
	const std::vector<std::string>& get_state_types() const;

	/// \brief Returns the state type index for the state part <tt>part</tt>.
	/// \param part the state part number.
	int get_state_type_no(int part) const;

	/// \brief Returns the number of state labels.
	size_t get_number_of_state_labels() const;

	/// \brief Returns the sequence of state labels.
	const std::vector<std::string>& get_state_labels() const;

	/// \brief Returns the sequence of state label types.
	const std::vector<std::string>& get_state_label_types() const;

	/// \brief Returns the number of edge labels.
	size_t get_number_of_edge_labels() const;

	/// \brief Returns the sequence of edge labels.
  const std::vector<std::string>& get_edge_labels() const;

  /// \brief Returns the sequence of edge label types.
  const std::vector<std::string>& get_edge_label_types() const;

  /// \brief Adds a state part of type <tt>type</tt> with name <tt>name</tt>.
  /// \param name the name of the state part.
  /// \param type the type of the state part.
	void add_state(const std::string& name, const std::string& type);

  /// \brief Adds a state label of type <tt>type</tt> with name <tt>name</tt>.
  /// \param name the name of the state label.
  /// \param type the type of the state label.
	void add_state_label(const std::string& name, const std::string& type);

  /// \brief Adds an edge label of type <tt>type</tt> with name <tt>name</tt>.
  /// \param name the name of the edge label.
  /// \param type the type of the edge label.
	void add_edge_label(const std::string& name, const std::string& type);
};




/// \brief
class ltsmin_state {
public:
    typedef core::term_traits<pbes_expression> tr;
    typedef parity_game_generator::operation_type operation_type;

private:
    int priority; // Priority (depends on fixpoint operator and equation order)
    std::string var; // Propositional variable name
    operation_type type; // player or type (And/Or, Abelard/Eloise, Odd/Even)
    atermpp::vector<data_expression>* param_values; // List of parameter values

public:
    /// \brief Constructor.
    /// \param priority the priority corresponding to the variable name.
    /// \param v the name of the propositional variable of the state.
    /// \param type the type or player of the state.
    /// \param e a propositional variable instantiation.
    ltsmin_state(int priority, const std::string& varname, operation_type type);

    /// \brief Constructor.
    /// \param priority the priority corresponding to the variable name.
    /// \param v the propositional variable of the state.
    /// \param type the type or player of the state.
    ltsmin_state(int priority, const propositional_variable& v, operation_type type);

    /// \brief Constructor.
    /// \param priority the priority corresponding to the variable name.
    /// \param v the propositional variable of the state.
    /// \param type the type or player of the state.
    /// \param e a propositional variable instantiation.
    ltsmin_state(int priority, const propositional_variable& v, operation_type type, const pbes_expression& e);

    /// \brief Destructor.
    ~ltsmin_state();

    /// \brief Compares two PBES_State objects. Uses lexicographical ordering on priority, type, variable and parameter values.
    /// \param other an other PBES_State object.
    /// \return true if this.priority < other.priority || (this.priority==other.priority && (this.type < other.type ||
    ///   (this.type==other.type && this.var < other.var || (this.var==other.var && this.param_values < other.param_values) ) ) ).
    bool operator<( const ltsmin_state& other ) const;

    /// \brief Checks if two PBES_State objects are equal.
    /// \param other an other PBES_State object.
    /// \return true if this.priority==other.priority && this.type==other.type && this.var==other.var
    /// && param_values==param_values.
    bool operator==( const ltsmin_state& other ) const;

    /// \brief Returns the priority for the state, which depends on the fixpoint operator of
    /// the equation of the propositional variable of the state and on the equation order.
    int get_priority() const;

    /// \brief Returns a string representation of the propositional variable of the state.
    std::string get_variable() const;

    /// \brief Returns the player or type of the state (And/Or, Abelard/Eloise, Odd/Even).
    operation_type get_type() const;

    /// \brief Returns the list of parameter values.
    atermpp::vector<data_expression>* get_parameter_values() const;

    /// \brief Adds a parameter value to the list of parameter values.
    void add_parameter_value(const data_expression&);

    /// \brief Returns a PBES expression representing the state.
    pbes_expression to_pbes_expression() const;

    /// \brief Returns a string representation of the state.
    std::string to_string() const;
};




/// \brief
class lts_info {
public:
    typedef core::term_traits<pbes_expression> tr;
    /// \brief The variable sequence type
    typedef parity_game_generator::operation_type operation_type;
private:
    pbes<>& p;
    pbes_greybox_interface* pgg;
    bool reset_option;
    lts_type type;
    std::map<int,std::vector<bool> > read_matrix;
    std::map<int,std::vector<bool> > write_matrix;
    std::map<int,std::vector<bool> > matrix;
    std::map<std::string,int> param_index;
    atermpp::vector<data_expression> param_default_values;
    int number_of_groups;
    atermpp::map<int,pbes_expression> transition_expression;
    std::map<int,std::string> transition_variable_name;
    atermpp::map<int,operation_type> transition_type;
    atermpp::map<std::string, propositional_variable> variables;
    atermpp::map<std::string, operation_type> variable_type;
    atermpp::map<std::string, fixpoint_symbol> variable_symbol;
    std::map<std::string, int> variable_priority;
    atermpp::map<std::string, data::variable_list> variable_parameters;
    std::map<std::string, std::vector<std::string> > variable_parameter_signatures;
    std::map<std::string, std::vector<int> > variable_parameter_indices;
    std::map<std::string, std::map<int,int> > variable_parameter_index_positions;

    /// \brief Computes LTS Type from PBES.
    void compute_lts_type();

    /// \brief Computes transition groups from PBES.
    void compute_transition_groups();

    /// \brief Computes dependency matrix from PBES.
    void compute_dependency_matrix();

public:
    /// \brief Constructor
    /// \param p
    /// \param pgg
    /// \param reset
    lts_info(pbes<>& p, pbes_greybox_interface* pgg, bool reset);

    /// \brief Destructor
    ~lts_info();

    /// \brief Returns if the reset option is set.
    bool get_reset_option() const;

    /// \brief Returns the number of transition groups.
    int get_number_of_groups() const;

    /// \brief Returns the map from transition group number to the expression of the transition group.
    const atermpp::map<int, pbes_expression>& get_transition_expressions() const;

    /// \brief Returns the map from transition group number to the variable name of the equation to which
    /// the transition group belongs.
    const std::map<int, std::string>& get_transition_variable_names() const;

    /// \brief Returns the map from transition group number to the type of the right hand side of the
    /// equation to which the transition group belongs.
    const atermpp::map<int, operation_type>& get_transition_types() const;

    /// \brief Returns the map from variable names to the variable object for the variable.
    const atermpp::map<std::string, propositional_variable>& get_variables() const;

    /// \brief Returns the map from variable names to the type of the right hand side of the equation for
    /// the variable.
    const atermpp::map<std::string, operation_type>& get_variable_types() const;

    /// \brief Returns the map from variable names to the fixpoint operator of the equation for the variable.
    const atermpp::map<std::string, fixpoint_symbol>& get_variable_symbols() const;

    /// \brief Returns the map from variable names to the priority of the equation for the variable.
    const std::map<std::string, int>& get_variable_priorities() const;

    /// \brief Returns the map from variable names to the sequence of parameters for the variable.
    const atermpp::map<std::string, data::variable_list>& get_variable_parameters() const;

    /// \brief Returns the map from variable names to the list of parameters signatures for the variable.
    const std::map<std::string, std::vector<std::string> >& get_variable_parameter_signatures() const;

    /// \brief Returns the map from variable names to the list of indices of the parameters signatures for the variable
    /// according the order in the list of parameter signatures for the system.
    const std::map<std::string, std::vector<int> >& get_variable_parameter_indices() const;

    /// \brief Returns the map from variable names to the map from indices of parameter signatures for the variable
    /// (according to the list of parameter signatures for the system) to the index of the type of the parameter
    /// (in the list of types for the system).
    const std::map<std::string, std::map<int,int> >& get_variable_parameter_index_positions() const;

    /// \brief Returns the LTS Type.
    const lts_type& get_lts_type() const;

    /// \brief Returns the dependency matrix.
    const std::map<int,std::vector<bool> >& get_dependency_matrix() const;

    /// \brief Returns the read dependency matrix.
    const std::map<int,std::vector<bool> >& get_read_matrix() const;

    /// \brief Returns the write dependency matrix.
    const std::map<int,std::vector<bool> >& get_write_matrix() const;

    /// \brief Returns the index for a parameter signature in the list of parameter signatures
    /// for the system.
    /// \param signature the parameter signature.
    int get_index(const std::string& signaturePBES_State);

    /// \brief Returns a default value for the sort of a parameter signature.
    /// \param index the index of the parameter signature.
    /// \return a default value for the sort of the parameter.
    const data_expression& get_default_value(int index);

    /// \brief Determines if <tt>group</tt> is read dependent on the propositional variable.
    /// Returns true, because the propositional variable is needed to determine if the group belongs
    /// to the variable.
    /// \param group the number of the transition group.
    /// \return true.
    bool is_read_dependent_propvar(int /* group */);

    /// \brief Determines if <tt>group</tt> is read dependent on part <tt>part</tt> of the state vector.
    /// Returns true if the parameter represented by <tt>part</tt> is in the set of parameters of the
    /// state variable for the group or in the set of data variables used in the expression of for the group.
    /// \param group the number of the transition group.
    /// \param part the number of the state part.
    /// \return true if param_part in ( params(var(group)) union used(expr(group)) ).
    bool is_read_dependent_parameter(int group, int part);

    /// \brief Determines if <tt>group</tt> is write dependent on the propositional variable.
    /// Returns true if propositional variables other than the group variable occur in the group expression
    /// or the group expression may directly evaluate to <tt>true</tt> or <tt>false</tt>.
    /// \param group the number of the transition group.
    /// \return true if ( occ(expr(group)) - {var(group)} ) is not empty or tf(expr(group)).
    bool is_write_dependent_propvar(int group);

    /// \brief Determines if <tt>group</tt> is read dependent on part <tt>part</tt> of the state vector.
    /// Returns true if the parameter represented by <tt>part</tt> is in the set of data variables
    /// changed by the expression for the group.
    /// \param group the number of the transition group.
    /// \param part the number of the state part.
    /// \return true if param_part in changed(expr(group)).
    bool is_write_dependent_parameter(int group, int part);

    /// \brief Computes the set of parameters changed in the expression.
    /// \param phi
    /// \param L
    std::set<std::string> changed(const pbes_expression& phi, const std::set<std::string>& L);

    /// \brief Computes the set of parameters reset in the expression.
    /// \param phi
    /// \param d
    std::set<std::string> reset(const pbes_expression& phi, const std::set<std::string>& d);

    /// \brief Determines if the term phi contains a branch that directly results in
    /// <tt>true</tt> or <tt>false</tt> (not a variable).
    /// \param phi a PBES expression
    static bool tf(const pbes_expression& phi);

    /// \brief Computes the propositional variables used in an expression.
    /// \param expr
    static std::set<std::string> occ(const pbes_expression& expr);

    /// \brief Computes the free variables read in an expression.
    /// \param expr
    static std::set<std::string> free(const pbes_expression& expr);

    /// \brief Computes the free variables actually used, not only passed through, in an expression.
    /// \param expr
    /// \param L
    std::set<std::string> used(const pbes_expression& expr);
    std::set<std::string> used(const pbes_expression& expr, const std::set<std::string>& L);

    /// \brief Returns a string representation for state <tt>state</tt>.
    /// \param state
    std::string to_string(const ltsmin_state& state);

    /// \brief Converts a variable_sequence_type into a set of parameter signatures.
    /// \param params a sequence of variables.
    static std::set<std::string> get_param_set(const data::variable_list& params);

    /// \brief Converts a variable_sequence_type into a sequence of parameter signatures.
    /// \param params a sequence of variables.
    static std::vector<std::string> get_param_sequence(const data::variable_list& params);

    /// \brief Converts a variable_sequence_type into a sequence of indices of parameter signatures
    /// in the list of parameter signatures for the system.
    /// \param params a sequence of variables.
    std::vector<int> get_param_indices(const data::variable_list& params);

    /// \brief Converts a variable_sequence_type into a map from indices of parameter signatures
    /// (in the list of parameter signatures for the system) to the index of the type of the parameter
    /// (in the list of types for the system).
    /// \param params a sequence of variables.
    std::map<int,int> get_param_index_positions(const data::variable_list& params);

    /// \brief Returns a signature for parameter <tt>param</tt>.
    /// \param param a parameter.
    static std::string get_param_signature(const variable& param);

    /// \brief Returns a signature using name and type of a parameter.
    /// \param paramname the parameter name.
    /// \param paramtype the parameter type.
    static std::string get_param_signature(const std::string& paramname, const std::string& paramtype);
};




/// \brief
class explorer {
public:
    /// \brief The expression type of the equation.
    typedef core::term_traits<pbes_expression> tr;
    typedef parity_game_generator::operation_type operation_type;

private:
    pbes<> p;
    lts_info* info;
    std::map<std::string,int> localmap_string2int;
    std::vector<std::string> localmap_int2string;
    atermpp::vector<atermpp::map<data_expression,int> > localmaps_data2int;
    atermpp::vector<atermpp::vector<data_expression> > localmaps_int2data;

public:
    /// \brief the PBES greybox interface
    pbes_greybox_interface* pgg;

    /// \brief Constructor.
    /// \param filename the name of a PBES file.
    explorer(const std::string& filename, data::rewrite_strategy rewrite_strategy, bool reset);

    /// \brief Constructor.
    /// \param p a PBES.
    explorer(const pbes<>& p, data::rewrite_strategy rewrite_strategy, bool reset);

    /// \brief Destructor.
    ~explorer();

    /// \brief Returns the PBES_Info object.
    lts_info* get_info() const;

    /// \brief Returns the initial state.
    ltsmin_state* get_initial_state() const;

    void initial_state(int* state);

    /// \brief Returns a PBES_State object for <tt>expr</tt>.
    /// \param expr a propositional variable instantiation expression.
    ltsmin_state* get_state(const propositional_variable_instantiation& expr) const;

    /// \brief Returns the state representing <tt>true</tt>.
    static inline ltsmin_state* true_state();

    /// \brief Returns the state representing <tt>false</tt>.
    static inline ltsmin_state* false_state();

    /// \brief Returns a string representation for the data expression <tt>e</tt>.
    /// \param e a PBES expression that may be in internal format.
    /// \return a string representation without internal rewriter quirks.
    std::string data_to_string(const data::data_expression& e);

    /// \brief Returns a data expression for the string representation <tt>s</tt>.
    /// \param s a string representation of a data expression.
    /// \return the data expression (possibly in internal format) that s represents.
    data::data_expression string_to_data(const std::string& s);

    /// \brief Returns the index of <tt>value</tt> in the local store for the data type
    /// with number <tt>type_no</tt>. Type 0 is reserved for the string representations
    /// of variable names.
    /// The value is added to the store if it is not already present.
    /// \param type_no the number of the value type.
    /// \param value a string representation of the data value.
    /// \return the index of <tt>value</tt> in local store <tt>type_no</tt>.
    int get_index(int type_no, const std::string& value);

    /// \brief Returns the index of <tt>s</tt> in the local store for string values.
    /// This store is reserved for the string representations of variable names.
    /// The value is added to the store if it is not already present.
    /// \param type_no the number of the value type.
    /// \param value the data value.
    /// \return the index of <tt>s</tt> in the local store for string values.
    int get_string_index(const std::string& s);

    /// \brief Returns the index of <tt>value</tt> in the local store for the data type
    /// with number <tt>type_no</tt>.
    /// The value is added to the store if it is not already present.
    /// \param type_no the number of the value type.
    /// \param value the data value.
    /// \return the index of <tt>value</tt> in local store <tt>type_no</tt>.
    int get_value_index(int type_no, const data_expression& value);

    /// \brief Transforms a PBES state to a state vector, represented by an array of integers.
    /// \param dst_state the new PBES state object
    /// \param dst the int array to which the state vector is written
    /// \param src_state the source PBES state object, used to check which fields have been changed.
    /// \param src an array which is used for default values, to prevent unused variables for being reset.
    ///        the non-resetting behaviour can be turned off by the --reset option.
    void to_state_vector(ltsmin_state* dst_state, int* dst, ltsmin_state* src_state, int* const& src);

    /// \brief Returns the value at position <tt>index</tt> in the local store for the data type
    /// with number <tt>type_no</tt>. Type 0 is reserved for the string representations
    /// of variable names.
    /// An exception is thrown if the index does not exist in the store.
    /// \param type_no the number of the value type.
    /// \param index an index.
    /// \return a string representation of the value at position <tt>index</tt> in local store <tt>type_no</tt>.
    std::string get_value(int type_no, int index);

    /// \brief Returns the string at position <tt>index</tt> in the local store for string values.
    /// An exception is thrown if the index does not exist in the store.
    /// \param index an index.
    /// \return the string value at position <tt>index</tt> in the local store for string values.
    const std::string& get_string_value(int index);

    /// \brief Returns the value at position <tt>index</tt> in the local store for the data type
    /// with number <tt>type_no</tt>.
    /// An exception is thrown if the index does not exist in the store.
    /// \param type_no the number of the value type.
    /// \param index an index.
    /// \return the value at position <tt>index</tt> in local store <tt>type_no</tt>.
    const data_expression& get_data_value(int type_no, int index);

    /// \brief Transforms a state vector <tt>src</tt> into a PBES_State object
    /// object containing the variable and parameter values that are represented
    /// by the indices in <tt>src</tt>.
    /// \param src an int array containg the indices of the state values.
    /// \return a PBES_State object containing the variable and parameter values that are represented
    /// by the indices in <tt>src</tt>.
    ltsmin_state* from_state_vector(int* const& src);

    /// \brief Computes successor states for a state. Serves as a wrapper around the get_successors
    /// function of the pbes_greybox_interface.
    /// \param state the source state.
    /// \return a list of successor states.
    atermpp::vector<ltsmin_state*> get_successors(const ltsmin_state& state);

    /// \brief Iterates over the successors of a state and invokes a callback
    /// function for each successor state.
    /// \param src an int array containg the indices of the state values.
    /// \param cb a callback function that must provide the function operator() with the following interface:
    ///
    /// \code
    /// void operator()(int* const& next_state, int group);
    /// \endcode
    /// where
    /// - next_state is the target state of the transition
    /// - group is the number of the transition group, or -1 if it is unknown which group
    template <typename callback>
        void next_state_all(int* const& src, callback& cb)
    {
        int state_length = this->info->get_lts_type().get_state_length();
        ltsmin_state* state = this->from_state_vector(src);
        //std::clog << "next_state_all: " << state->to_string() << std::endl;
        atermpp::vector<ltsmin_state*> successors = this->get_successors(*state);
        for (atermpp::vector<ltsmin_state*>::iterator succ = successors.begin(); succ
                != successors.end(); ++succ) {
            int dst[state_length];
            this->to_state_vector(*succ, dst, state, src);
            cb(dst);
            //std::clog << "  succ: " << (*succ)->to_string() << std::endl;
            delete *succ;
        }
        delete state;
    }

    /// \brief Computes successor states for a state as defined in transition group <tt>group</tt>.
    /// Serves as a wrapper around the get_successors function of the pbes_greybox_interface.
    /// \param state the source state.
    /// \param group the group for which the successor states are computed.
    /// \return a list of successor states.
    atermpp::vector<ltsmin_state*> get_successors(const ltsmin_state& state, int group);

    /// \brief Iterates over the successors of a state for a certain transition group
    /// and invokes a callback function for each successor state.
    /// \param src an int array containg the indices of the state values.
    /// \param group the transition group
    /// \param cb a callback function that must provide the function operator() with the following interface:
    ///
    /// \code
    /// void operator()(int* const& next_state, int group);
    /// \endcode
    /// where
    /// - next_state is the target state of the transition
    /// - group is the number of the transition group, or -1 if it is unknown which group
    template <typename callback>
        void next_state_long(int* const& src, int group, callback& cb)
    {
        int state_length = this->info->get_lts_type().get_state_length();
        ltsmin_state* state = this->from_state_vector(src);
        atermpp::vector<ltsmin_state*> successors = this->get_successors(*state, group);
        for (atermpp::vector<ltsmin_state*>::iterator succ = successors.begin(); succ
                != successors.end(); ++succ) {
            int dst[state_length];
            this->to_state_vector(*succ, dst, state, src);
            cb(dst, group);
            delete *succ;
        }
        delete state;
    }

};

} // namespace pbes_system

} // namespace mcrl2

#endif // PBES_EXPLORER_H
