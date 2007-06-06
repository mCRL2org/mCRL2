#include <string>
#include "libprint_c.h"
#include "lts/liblts.h"
#include "setup.h"

#ifdef MCRL2_BCG

#include <bcg_user.h>

// BCG library initialisation
bool initialise() {
  BCG_INIT();

  return true;
}

static bool initialised = initialise();

using namespace std;

namespace mcrl2
{
namespace lts
{

bool p_lts::read_from_bcg(string const& filename)
{
  string::size_type pos = filename.rfind('.');
  if ( (pos == string::npos) || (filename.substr(pos+1) != "bcg") )
  {
    gsVerboseMsg("cannot open BCG file without '.bcg' extension\n");
    return false;
  }

  BCG_TYPE_OBJECT_TRANSITION bcg_graph;

  BCG_OT_READ_BCG_SURVIVE(BCG_TRUE);
  char *s = strdup(filename.c_str());
  BCG_OT_READ_BCG_BEGIN(
      s,
      &bcg_graph,
      0
      );
  free(s);
  BCG_OT_READ_BCG_SURVIVE(BCG_FALSE);

  if ( bcg_graph == NULL )
  {
    gsVerboseMsg("could not open BCG file '%s' for reading\n",filename.c_str());
    return false;
  }

  unsigned int n;

  n = BCG_OT_NB_STATES(bcg_graph);
  for (unsigned int i=0; i<n; i++)
  {
    p_add_state();
  }
  init_state = BCG_OT_INITIAL_STATE(bcg_graph);

  n = BCG_OT_NB_LABELS(bcg_graph);
  for (unsigned int i=0; i<n; i++)
  {
    p_add_label((ATerm) ATmakeAppl0(ATmakeAFun(BCG_OT_LABEL_STRING(bcg_graph,i),0,ATtrue)),!strcmp(BCG_OT_LABEL_STRING(bcg_graph,i),"i"));
  }

  unsigned int from,label,to;
  BCG_OT_ITERATE_PLN(bcg_graph,from,label,to)
  {
   p_add_transition(from,label,to); 
  }
  BCG_OT_END_ITERATE;

  creator = "";
  type = lts_bcg;

  return true;
}

bool p_lts::write_to_bcg(string const& filename)
{
  BCG_IO_WRITE_BCG_SURVIVE(BCG_TRUE);
  char *s = strdup(filename.c_str());
  char *t = strdup(creator.c_str());
  BCG_TYPE_BOOLEAN b = BCG_IO_WRITE_BCG_BEGIN(
      s,
      init_state,
      1, // XXX add check to see if this might be 2?
      (creator != "")?t:NULL,
      0
      );
  free(t);
  free(s);
  BCG_IO_WRITE_BCG_SURVIVE(BCG_FALSE);

  if ( b == BCG_TRUE )
  {
    gsVerboseMsg("could not open BCG file '%s' for writing\n",filename.c_str());
    return false;
  }

  char *buf = NULL;
  unsigned int buf_size = 0;
  for (unsigned int i=0; i<ntransitions; i++)
  {
    string label_str = p_label_value_str(transitions[i].label);
    if ( label_str.size() > buf_size )
    {
      if ( buf_size == 0 )
      {
        buf_size = 128;
      }
      while ( label_str.size() > buf_size )
      {
        buf_size = 2 * buf_size;
      }
      buf = (char *) realloc(buf,buf_size);
      if ( buf == NULL )
      {
        gsErrorMsg("insufficient memory to write LTS to BCG\n");
        exit(1);
      }
    }
    strcpy(buf,label_str.c_str());
    BCG_IO_WRITE_BCG_EDGE(transitions[i].from,buf,transitions[i].to);
  }

  BCG_IO_WRITE_BCG_END();

  return true;
}

}
}

#endif
