// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tools.cpp

#include <fstream>

#include "mcrl2/exception.h"
#include "mcrl2/lps/constelm.h"
#include "mcrl2/lps/detail/specification_property_map.h"
#include "mcrl2/lps/parelm.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/remove.h"
#include "mcrl2/lps/rewrite.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{

namespace lps
{

void lpsconstelm(const std::string& input_filename,
                 const std::string& output_filename,
                 data::rewriter::strategy rewrite_strategy,
                 bool instantiate_free_variables,
                 bool ignore_conditions,
                 bool remove_trivial_summands,
                 bool remove_singleton_sorts
                )
{
  lps::specification spec;
  spec.load(input_filename);
  mcrl2::data::rewriter R(spec.data(), rewrite_strategy);
  lps::constelm_algorithm<data::rewriter> algorithm(spec, R);

  // preprocess: remove single element sorts
  if (remove_singleton_sorts)
  {
    algorithm.remove_singleton_sorts();
  }

  // apply constelm
  algorithm.run(instantiate_free_variables, ignore_conditions);

  // postprocess: remove trivial summands
  if (remove_trivial_summands)
  {
    algorithm.remove_trivial_summands();
  }

  spec.save(output_filename);
}

void lpsinfo(const std::string& input_filename,
             const std::string& input_file_message
            )
{
  specification spec;
  spec.load(input_filename);
  lps::detail::specification_property_map info(spec);
  std::cout << input_file_message << "\n\n";
  std::cout << info.info();
}

void lpsparelm(const std::string& input_filename,
               const std::string& output_filename
              )
{
  lps::specification spec;
  spec.load(input_filename);
  lps::parelm(spec, true);
  spec.save(output_filename);
}

void lpspp(const std::string& input_filename,
           const std::string& output_filename,
           bool print_summand_numbers,
           core::t_pp_format format
          )
{
  lps::specification spec;
  spec.load(input_filename);

  mCRL2log(log::verbose) << "printing LPS from "
                    << (input_filename.empty()?"standard input":input_filename)
                    << " to " << (output_filename.empty()?"standard output":output_filename)
                    << " in the " << core::pp_format_to_string(format) << " format" << std::endl;

  std::string text;
  if (format == core::ppInternal)
  {
  	text = specification_to_aterm(spec).to_string();
  }
  else
  {
  	text = print_summand_numbers ? lps::pp_with_summand_numbers(spec) : lps::pp(spec);
  }
  if (output_filename.empty())
  {
  	std::cout << text;
  }
  else
  {
    std::ofstream output_stream(output_filename.c_str());
    if (output_stream)
    {
      output_stream << text;
      output_stream.close();
    }
    else
    {
      throw mcrl2::runtime_error("could not open output file " + output_filename + " for writing");
    }
  }
}

template <typename DataRewriter>
void lpsrewr_bench_mark(const lps::specification& spec, const DataRewriter& R, unsigned long bench_times)
{
  std::clog << "rewriting LPS " << bench_times << " times...\n";
  for (unsigned long i=0; i < bench_times; i++)
  {
    lps::specification spec1 = spec;
    lps::rewrite(spec1, R);
  }
}

// TODO: remove the benchmark option?
void lpsrewr(const std::string& input_filename,
             const std::string& output_filename,
             const data::rewriter::strategy rewrite_strategy,
             bool benchmark,
             unsigned long bench_times
            )
{
  lps::specification spec;
  spec.load(input_filename);
  mcrl2::data::rewriter R(spec.data(), rewrite_strategy);
  if (benchmark)
  {
    lpsrewr_bench_mark(spec, R, bench_times);
  }
  lps::rewrite(spec, R);
  lps::remove_trivial_summands(spec);
  spec.save(output_filename);
}

void txtlps(const std::string& input_filename,
            const std::string& output_filename
           )
{
  lps::specification spec;
  if (input_filename.empty())
  {
    spec = lps::parse_linear_process_specification(std::cin);
  }
  else
  {
    std::ifstream from(input_filename.c_str());
    if (!from)
    {
      throw mcrl2::runtime_error("Could not read from input file: " + input_filename);
    }
    spec = lps::parse_linear_process_specification(from);
  }
  spec.save(output_filename);
}

} // namespace lps

} // namespace mcrl2
