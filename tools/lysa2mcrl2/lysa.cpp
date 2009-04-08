// Author(s): Egbert Teeselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lysa.cpp
/// \brief Add your file description here.

#include "lysa.h"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <stack>

using namespace boost;
using namespace std;

namespace lysa
{
	shared_ptr<ProcessInfo> current_process_info;
	void start_parsing(lysa_options &options)
	{
		shared_ptr<ProcessInfo> p(new ProcessInfo(options));
		current_process_info = p;
	}
	void set_current_position(parse_location &pos)
	{
		current_process_info->current_col = pos.first_column;
		current_process_info->current_line = pos.first_line;
	}
	
	string calculus_names[2] = {"LySa", "Typed LySa"};

	void ProcessInfo::set_calculus(Calculus c)
	{
		if(_calculus==Unknown)
		{
			_calculus = c;
			gsVerboseMsg("recognised %s syntax.\n", calculus_names[c].c_str());
		}
		else
		{
			if(_calculus!=c)
			{
				throw "recognised " + calculus_names[c] + " syntax, which conflicts with previously recognised " + calculus_names[_calculus] + " syntax.";
			}
		}
	}
	void ProcessInfo::override_calculus(Calculus c)
	{ 
		_calculus = c;
	}

	template< typename T>	string join_ptr(T& input, string sep)
	{
		string s = "";
		typename T::iterator it=input.begin();
		typename T::iterator it_end=input.end();

		if(it==it_end)
		{
			return s;
		}
		//write first element
		s += (string)(**it);
		it++;

		//write other elements

		for(;it!=it_end;it++)
		{
			s += sep + (string)(**it);
		}
		return s;
	}
	template< typename T>	string join(T& input, string sep)
	{
		string s = "";
		typename T::iterator it=input.begin();
		typename T::iterator it_end=input.end();

		if(it==it_end)
		{
			return s;
		}
		//write first element
		s += (string)(*it);
		it++;

		//write other elements

		for(;it!=it_end;it++)
		{
			s += sep + (string)(*it);
		}
		return s;
	}
	//template string join(Indices& input, string sep);


	string Expression::position_in_input()
	{
		ostringstream s;
		s << "at position " << line_in_input << ", " << col_in_input;
		return s.str();
	}

  vector<E_ptr> Expression::subexpressions()
  {
    vector<E_ptr> v;
    return v;
  }

#define S(v) ((string)(*(v)))


  //find the *last* OrdinaryParallel before a DY is encountered
  E_ptr Expression::find_opar_before_dy(E_ptr last_opar)
  {
    BOOST_FOREACH(E_ptr e, subexpressions())
    {
      if(dynamic_pointer_cast<OrdinaryParallel>(e))
      {
        last_opar = e;
      }
      E_ptr r = e->find_opar_before_dy(last_opar);
      if(r.get())
      {
        return r;
      }
    }
    E_ptr empty;
    return empty;
  }
  E_ptr DY::find_opar_before_dy(E_ptr last_opar)
  {
    return last_opar;
  }

  //find all Let objects but does not search into the tree that starts at "stop_at";
  std::list<E_ptr> Expression::find_let_until(E_ptr stop_at)
  {
    std::list<E_ptr> l;
    BOOST_FOREACH(E_ptr e, subexpressions())
    {
      if(e!=stop_at)
      {
        std::list<E_ptr> sl = e->find_let_until(stop_at);
        l.insert(l.end(), sl.begin(), sl.end());
      }
      if(dynamic_pointer_cast<Let>(e))
      {
        l.push_back(e);
      }
    }
    return l;
  }

  string Expression::typed_lysa_to_lysa()
  {
    if(this->process_info->calculus()==TypedLySa)
    {
      this->process_info->override_calculus(LySa);
      E_ptr empty;
      E_ptr opar = find_opar_before_dy(empty);
      if(opar.get())
      {
        std::list<E_ptr> lets = find_let_until(opar);
        string s;
        BOOST_FOREACH(E_ptr let, lets)
        {
          s += dynamic_pointer_cast<Let>(let)->to_string_without_deepening();
        }
        return s + S(opar);
      }
      else
      {
        gsWarningMsg("Cannot find attacker. Converting entire process to LySa.");
        return S(this);
      }
    }
    else
    {
      gsWarningMsg("Input process detected as LySa when trying to convert Typed LySa to LySa");
      return S(this);
    }
  }


	Indices::operator string()
	{
		return join(*this, ", ");
	}
	Indices::Indices(E_ptr i)
	{
		std::string s = static_pointer_cast<String>(i)->s;
		push_back(s);
	}
	//version for splitting every character into a separate index
	Indices::Indices(E_ptr s, bool isShort)
	{
		BOOST_FOREACH(char c, static_pointer_cast<String>(s)->s)
		{
			string q = "";
			q += c;
			push_back(q);
		}
	}
	void Indices::push_back(string s)
	{
		string_list::push_back(process_info->options.prefix + s);
	}

	IndexDef::operator string() 
	{ 
		return index + " in " + set;
	}
	IndexDefs::operator string()
	{
		return join_ptr(*this, ", ");
	}
	IndexDefs::IndexDefs(E_ptr id)
	{
		push_back(static_pointer_cast<IndexDef>(id));
	}
	void IndexDefs::push_back(E_ptr id)
	{
		IndexDef_list::push_back(static_pointer_cast<IndexDef>(id));
	}

	Terms::Terms(E_ptr t)
	{
		push_back(t);
	}
	void Terms::push_back(E_ptr t)
	{
		Term_list::push_back(static_pointer_cast<Term>(t));
	}
	Terms::operator string()
	{
		return join_ptr(*this, ", ");
	}
  vector<E_ptr> Terms::subexpressions() 
  { 
    return vector<E_ptr>(begin(),end()); 
  };


	Identifier::operator string() 
	{ 
		return name + ((indices->empty()) ? "" : "_{" + S(indices) + "}"); 
	}

	
	ASymName::operator string() 
	{ 
		return name + ((hasPlus) ? "+" : "-") + ((indices->empty()) ? "" : "_{" + S(indices) + "}"); 
	}

	string var_defs[3] = {"", " : N", " : C"};
	TypedVar::operator string() 
	{ 
    return (Identifier::operator string()) + ((process_info->calculus()==TypedLySa) ? var_defs[type] : "");
	}

	Ciphertext::operator string() 
	{ 
		return ((isASym)?"{|":"{") + S(terms) + ((isASym)?"|}":"}") + " : " + S(key) + " " + S(anno);
	}
  vector<E_ptr> Ciphertext::subexpressions() 
  { 
    vector<E_ptr> v;
    v.push_back(terms); 
    v.push_back(key); 
    v.push_back(anno); 
    return v; 
  };
	
	IsetIndices::operator string() 
	{ 
		return "{" + S(indices) + "}";
	}
	domain IsetIndices::to_domain()
	{
		domain d;
		BOOST_FOREACH(string i, *indices)
		{
			try
			{
				d.insert(lexical_cast<int>(i));
			}
			catch(bad_lexical_cast &)
			{
				throw "unexpected error: cannot convert index '" + lexical_cast<string>(i) + "' to set domain.\n";
			}
		}
		return d;
	}
  vector<E_ptr>IsetIndices::subexpressions() 
  { 
    vector<E_ptr> v; 
    v.push_back(indices); 
    return v; 
  };

	string iset_defs[7] = {"ZERO", "NATURAL1", "NATURAL2", "NATURAL3", "NATURAL01", "NATURAL02", "NATURAL03"};
	IsetDef::operator string() 
	{ 
		return iset_defs[(int)def_set];
	}

	domain IsetDef::to_domain()
	{
		domain d;
		switch(def_set)
		{
		case NATURAL3:
		case NATURAL03:
			d.insert(3);
		case NATURAL2:
		case NATURAL02:
			d.insert(2);
		case NATURAL1:
		case NATURAL01:
			d.insert(1);
                default: break;
		}
		if((def_set==ZERO) || (def_set >= NATURAL01))
		{
			d.insert(0);
		}
		return d;
	}
	IsetUnion::operator string() 
	{ 
		return S(iset_left) + " union " + S(iset_right);
	}
	domain IsetUnion::to_domain()
	{
		domain l = iset_left->to_domain();
		domain r = iset_right->to_domain();
		l.insert(r.begin(), r.end());
		return l;
	}
  vector<E_ptr> IsetUnion::subexpressions() 
  { 
    vector<E_ptr> v; 
    v.push_back(iset_left); 
    v.push_back(iset_right); 
    return v; 
  };

  DY::operator string()
  { 
    return string((process_info->calculus()==TypedLySa) ? "DY" : "0") + "\n";
  }

  string Let::to_string_without_deepening()
  {
    return "let " + name + ((process_info->calculus()==LySa) ? " subset " : " = ") +
			S(iset) + " in\n";
  }
	Let::operator string() 
	{ 
		return to_string_without_deepening() + S(proc);
	}

  vector<E_ptr> Let::subexpressions() 
  { 
    vector<E_ptr> v; 
    v.push_back(iset); 
    v.push_back(proc); 
    return v; 
  };

	Send::operator string() 
	{ 
		return "<" + S(terms) + ">.\n" + S(proc);
	}
  vector<E_ptr> Send::subexpressions() 
  { 
    vector<E_ptr> v;
    v.push_back(terms); 
    v.push_back(proc); 
    return v; 
  };

	PMatchTerms::PMatchTerms(E_ptr t, E_ptr v) : match_terms(static_pointer_cast<Terms>(t)), vars(static_pointer_cast<Terms>(v)) 
	{
		//ensure that types are specified if and only if we've typed lysa
		BOOST_FOREACH(shared_ptr<Term> term, *vars)
		{
			process_info->set_calculus(
				(dynamic_pointer_cast<TypedVar>(term)) ? TypedLySa : LySa
				);
		}
	}

	PMatchTerms::operator string()
	{
		return S(match_terms) + "; " + S(vars);
	}
  vector<E_ptr> PMatchTerms::subexpressions() 
  { 
    vector<E_ptr> v;
    v.push_back(match_terms); 
    v.insert(v.end(),vars->begin(), vars->end()); 
    return v; 
  };


	Receive::operator string() 
	{ 
		return "(" + S(terms) + ").\n" + S(proc);
	}
  vector<E_ptr> Receive::subexpressions() 
  { 
    vector<E_ptr> v;
    v.push_back(terms); 
    v.push_back(proc); 
    return v; 
  };

	Decrypt::operator string() 
	{ 
		return "decrypt " + S(ciphertext) + " as {" + S(terms) + "} : " + S(key) + 
			" " + S(anno) + " in \n" + S(proc);
	}
  vector<E_ptr> Decrypt::subexpressions() 
  { 
    vector<E_ptr> v; 
    v.push_back(ciphertext); 
    v.push_back(terms); 
    v.push_back(key); 
    v.push_back(proc); 
    v.push_back(anno); 
    return v; 
  };

	New::operator string() 
	{ 
		return "(new" + ((index_defs->empty()) ? "" : "_{" + S(index_defs) + "}") +
			((isASym)?"+-":"") + " " + S(name) + ") (\n" + S(proc) + "\n)";
	}
  vector<E_ptr> New::subexpressions() 
  { 
    vector<E_ptr> v;
    v.push_back(name); 
    v.push_back(proc); 
    v.push_back(index_defs); 
    return v; 
  };

	Replication::operator string() 
	{
		return "!" + S(proc);
	}
  vector<E_ptr> Replication::subexpressions() 
  { 
    vector<E_ptr> v;
    v.push_back(proc); 
    return v; 
  };

	IndexedParallel::operator string() 
	{
    if(process_info->calculus()==TypedLySa)
    {
		  return "|_{" + S(index_defs) + "} \n" + S(proc);
    }
    else
    {
      string s;
      shared_ptr<IndexDef> id;
      BOOST_FOREACH(id, *index_defs)
      {
        s += "|_{" + S(id) + "} ";
      }
      return s + "\n" + S(proc);
    }
	}
  vector<E_ptr> IndexedParallel::subexpressions() 
  { 
    vector<E_ptr> v;
    v.push_back(proc); 
    v.push_back(index_defs); 
    return v; 
  };

	OrdinaryParallel::OrdinaryParallel(std::list<E_ptr> ps)
	{
		BOOST_FOREACH(E_ptr p, ps)
		{
			//put nested ordinary parallel operators in one list. this is semantics preserving, because
			//the parallel operator is associative.
			if(shared_ptr<OrdinaryParallel> op = dynamic_pointer_cast<OrdinaryParallel>(p))
			{
				BOOST_FOREACH(shared_ptr<Proc> subproc, op->procs)
				{
					procs.push_back(static_pointer_cast<Proc>(subproc));
				}
			}
			else
			{
				procs.push_back(static_pointer_cast<Proc>(p));
			}
		}
	}

	OrdinaryParallel::operator string() 
	{
		//return "(" + S(left_proc) + ") | (" + S(right_proc) + ")";
		return "(\n" + join_ptr(procs, ")\n| (\n") + ")\n";
	}
  vector<E_ptr> OrdinaryParallel::subexpressions() 
  { 
    vector<E_ptr> v(procs.begin(), procs.end());
    return v; 
  };

	Cryptopoints::Cryptopoints(E_ptr c)
	{
		push_back(c);
	}
	void Cryptopoints::push_back(E_ptr c)
	{
		Cryptopoint_list::push_back(static_pointer_cast<Cryptopoint>(c));
	}
	Cryptopoints::operator string()
	{
		return join_ptr(*this, ", ");
	}
  vector<E_ptr> Cryptopoints::subexpressions() 
  { 
    vector<E_ptr> v(begin(), end());
    return v; 
  };

	Annotation::Annotation(bool isDest, E_ptr a, E_ptr d_o) : at(static_pointer_cast<Cryptopoint>(a)), isDest(isDest), dest_orig(static_pointer_cast<Cryptopoints>(d_o))
	{
		//dest_orig->push_back(d_o);
	};
	Annotation::operator string()
	{
		if(at.get())
		{
			if(dest_orig->empty())
			{
				return "[at " + S(at) + "]";
			}
			else
			{
				return "[at " + S(at) + (isDest ? " dest {" : " orig {") + S(dest_orig) + "}]";
			}
		}
		else
		{
			return "";
		}
	}
  vector<E_ptr> Annotation::subexpressions() 
  { 
    vector<E_ptr> v;
    v.push_back(at);
    v.push_back(dest_orig);
    return v; 
  };
}
