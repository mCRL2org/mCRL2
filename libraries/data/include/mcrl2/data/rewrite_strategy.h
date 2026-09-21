#ifndef MCRL2_DATA_REWRITE_STRATEGY_H
#define MCRL2_DATA_REWRITE_STRATEGY_H

#include "mcrl2/utilities/exception.h"
#include <string>

namespace mcrl2::data
{

/// \brief The strategy of the rewriter.
enum class rewrite_strategy
{
  jitty,                      /** \brief JITty */
#ifdef MCRL2_ENABLE_JITTYC
  jitty_compiling,            /** \brief Compiling JITty */
  jitty_prover,               /** \brief JITty + Prover */
  jitty_compiling_prover      /** \brief Compiling JITty + Prover*/
#else
  jitty_prover                /** \brief JITty + Prover */
#endif
};

/// \brief standard conversion from string to rewrite strategy
inline
rewrite_strategy parse_rewrite_strategy(const std::string& s)
{
  if (s == "jitty")
  {
    return rewrite_strategy::jitty;
  }
  else if (s == "jittyp")
  {
    return rewrite_strategy::jitty_prover;
  }

#ifdef MCRL2_ENABLE_JITTYC
  if (s == "jittyc")
  {
    return rewrite_strategy::jitty_compiling;
  }
  else if (s == "jittycp")
  {
    return rewrite_strategy::jitty_compiling_prover;
  }
#endif //MCRL2_ENABLE_JITTYC

  throw mcrl2::runtime_error("unknown rewrite strategy " + s);
}

/// \brief standard conversion from stream to rewrite strategy
inline std::istream& operator>>(std::istream& is, rewrite_strategy& s)
{
  try
  {
    std::stringbuf buffer;
    is >> &buffer;
    s = parse_rewrite_strategy(buffer.str());
  }
  catch(mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }

  return is;
}

/// \brief Pretty prints a rewrite strategy
/// \param[in] s A rewrite strategy.
inline std::string pp(const rewrite_strategy s)
{
  switch (s)
  {
    case rewrite_strategy::jitty: return "jitty";
#ifdef MCRL2_ENABLE_JITTYC
    case rewrite_strategy::jitty_compiling: return "jittyc";
#endif
    case rewrite_strategy::jitty_prover: return "jittyp";
#ifdef MCRL2_ENABLE_JITTYC
    case rewrite_strategy::jitty_compiling_prover: return "jittycp";
#endif
    default: throw mcrl2::runtime_error("unknown rewrite_strategy");
  }
}

/// \brief standard conversion from rewrite strategy to stream
inline std::ostream& operator<<(std::ostream& os, const rewrite_strategy s)
{
  os << pp(s);
  return os;
}

/// \brief standard descriptions for rewrite strategies
inline std::string description(const rewrite_strategy s)
{
  switch (s)
  {
    case rewrite_strategy::jitty: return "jitty rewriting";
#ifdef MCRL2_ENABLE_JITTYC
    case rewrite_strategy::jitty_compiling: return "compiled jitty rewriting";
#endif
    case rewrite_strategy::jitty_prover: return "jitty rewriting with prover";
#ifdef MCRL2_ENABLE_JITTYC
    case rewrite_strategy::jitty_compiling_prover: return "compiled jitty rewriting with prover";
#endif
    default: throw mcrl2::runtime_error("unknown rewrite_strategy");
  }
}

} // namespace mcrl2::data

#endif // MCRL2_DATA_REWRITE_STRATEGY_H
