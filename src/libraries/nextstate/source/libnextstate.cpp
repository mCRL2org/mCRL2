#include <aterm2.h>
#include "libnextstate.h"
#include "nextstate_standard.h"

NextStateGenerator::~NextStateGenerator()
{
}

NextState::~NextState()
{
}

NextState *createNextState(
  ATermAppl spec,
  bool allow_free_vars,
  int state_format,
  Enumerator *e,
  bool clean_up_enumerator,
  NextStateStrategy strategy
)
{
  switch ( strategy )
  {
    case nsStandard:
      return new NextStateStandard(spec,allow_free_vars, state_format,e,clean_up_enumerator);
    default:
      if ( clean_up_enumerator )
      {
        delete e;
      }
    return NULL;
  }
}

NextState *createNextState(
  ATermAppl spec,
  bool allow_free_vars,
  int state_format,
  RewriteStrategy rewrite_strategy,
  EnumerateStrategy enumerator_strategy,
  NextStateStrategy strategy
)
{
  return
    createNextState(
      spec,
      allow_free_vars,
      state_format,
      createEnumerator(
        spec,
        createRewriter(
          lpe::data_specification((ATermAppl) ATgetArgument(spec, 0)),
          rewrite_strategy
        ),
        true,
        enumerator_strategy),
      true,
      strategy
    );
}
