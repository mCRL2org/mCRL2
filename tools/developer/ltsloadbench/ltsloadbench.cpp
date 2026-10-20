// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsloadbench.cpp
/// \brief Measures the separate layers involved in loading an LTS from disk, and saving it.

#include <chrono>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <sys/resource.h>

#include "mcrl2/atermpp/aterm_io_binary.h"
#include "mcrl2/data/detail/io.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/utilities/input_tool.h"

using namespace mcrl2;

namespace
{

/// \returns The peak resident set size of this process in MiB.
double peak_rss_mib()
{
  struct rusage usage{};
  getrusage(RUSAGE_SELF, &usage);
#ifdef __APPLE__
  return static_cast<double>(usage.ru_maxrss) / (1024.0 * 1024.0); // ru_maxrss is in bytes on macOS
#else
  return static_cast<double>(usage.ru_maxrss) / 1024.0; // ru_maxrss is in KiB on Linux
#endif
}

std::ifstream open_binary(const std::string& filename)
{
  std::ifstream stream(filename, std::ifstream::binary);
  if (!stream)
  {
    throw mcrl2::runtime_error("Cannot open the file " + filename + ".");
  }
  return stream;
}

/// \brief Reads the file into memory in blocks of 1 MiB; the baseline for pure I/O.
void run_raw(const std::string& filename)
{
  std::ifstream stream = open_binary(filename);
  std::vector<char> buffer(std::size_t(1) << 20);
  std::size_t total = 0;
  while (stream.read(buffer.data(), static_cast<std::streamsize>(buffer.size())) || stream.gcount() > 0)
  {
    total += static_cast<std::size_t>(stream.gcount());
  }
  std::cout << "bytes: " << total << "\n";
}

/// \brief Decodes all top-level terms in the file without building an LTS.
void run_decode(const std::string& filename)
{
  std::ifstream fstream = open_binary(filename);
  atermpp::binary_aterm_istream stream(fstream);
  // Use the same transformer as the LTS reader, so that term construction is comparable.
  stream >> data::detail::add_index_impl;

  std::size_t number_of_terms = 0;
  atermpp::aterm term;
  while (true)
  {
    stream.get(term);
    if (!term.defined())
    {
      break;
    }
    ++number_of_terms;
  }
  std::cout << "top-level terms: " << number_of_terms << "\n";
}

template<typename LTS>
void run_load(const std::string& filename)
{
  LTS lts;
  lts.load(filename);
  std::cout << "states: " << lts.num_states() << "\n"
            << "transitions: " << lts.num_transitions() << "\n"
            << "action labels: " << lts.num_action_labels() << "\n"
            << "state labels: " << lts.num_state_labels() << "\n";
}

/// \brief Loads the LTS and saves it to output_filename; only the saving is timed.
/// \returns The moment the saving started.
template<typename LTS>
std::chrono::steady_clock::time_point run_save(const std::string& filename, const std::string& output_filename)
{
  LTS lts;
  lts.load(filename);
  const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
  lts.save(output_filename);
  std::cout << "transitions: " << lts.num_transitions() << "\n"
            << "state labels: " << lts.num_state_labels() << "\n";
  return start;
}

} // namespace

class ltsloadbench_tool : public utilities::tools::input_tool
{
protected:
  using super = utilities::tools::input_tool;
  std::string m_mode = "load";
  std::string m_output_filename = "/dev/null";

  void parse_options(const utilities::command_line_parser& parser) override
  {
    super::parse_options(parser);
    if (parser.has_option("mode"))
    {
      m_mode = parser.option_argument("mode");
    }
    if (parser.has_option("output"))
    {
      m_output_filename = parser.option_argument("output");
    }
    if (m_mode != "raw" && m_mode != "decode" && m_mode != "load" && m_mode != "pload" && m_mode != "aut"
        && m_mode != "save" && m_mode != "psave")
    {
      parser.error("unknown mode '" + m_mode + "'");
    }
  }

  void add_options(utilities::interface_description& desc) override
  {
    super::add_options(desc);
    desc.add_option("mode",
      utilities::make_optional_argument("MODE", "load"),
      "what to measure: 'raw' reads the file into memory (I/O baseline), "
      "'decode' decodes all aterms without building an LTS, "
      "'load' loads the file as an lts_lts_t (default), "
      "'pload' loads it as a probabilistic_lts_lts_t, "
      "'aut' loads an .aut file as an lts_aut_t, "
      "'save' loads the file as an lts_lts_t and measures saving it to the file given by --output, "
      "'psave' does the same with a probabilistic_lts_lts_t",
      'm');
    desc.add_option("output",
      utilities::make_optional_argument("FILE", "/dev/null"),
      "the file to which the modes 'save' and 'psave' write (default /dev/null)",
      'o');
  }

public:
  ltsloadbench_tool()
    : super("ltsloadbench",
        "Jeroen Keiren",
        "benchmark the loading of labelled transition systems",
        "Loads INFILE in the way selected by --mode, and prints the elapsed time, the peak resident set size, and "
        "statistics about what was read. In the modes 'save' and 'psave', only the time to save the LTS is "
        "reported.")
  {}

  bool run() override
  {
    if (input_filename().empty())
    {
      throw mcrl2::runtime_error("ltsloadbench requires an input file.");
    }

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    if (m_mode == "raw")
    {
      run_raw(input_filename());
    }
    else if (m_mode == "decode")
    {
      run_decode(input_filename());
    }
    else if (m_mode == "load")
    {
      run_load<lts::lts_lts_t>(input_filename());
    }
    else if (m_mode == "pload")
    {
      run_load<lts::probabilistic_lts_lts_t>(input_filename());
    }
    else if (m_mode == "aut")
    {
      run_load<lts::lts_aut_t>(input_filename());
    }
    else if (m_mode == "save")
    {
      start = run_save<lts::lts_lts_t>(input_filename(), m_output_filename);
    }
    else
    {
      start = run_save<lts::probabilistic_lts_lts_t>(input_filename(), m_output_filename);
    }
    const std::chrono::duration<double, std::milli> elapsed = std::chrono::steady_clock::now() - start;

    std::cout << "mode: " << m_mode << "\n"
              << "time_ms: " << elapsed.count() << "\n"
              << "peak_rss_mib: " << peak_rss_mib() << "\n";
    return true;
  }
};

int main(int argc, char* argv[])
{
  return ltsloadbench_tool().execute(argc, argv);
}
