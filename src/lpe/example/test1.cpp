#include <string>
#include "atermpp/aterm.h"

using namespace std;
using namespace atermpp;

int main()
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);

  string filename("data/abp_b.lpe");
  aterm_appl t = read_from_named_file(filename).to_aterm_appl();
  if (!t)
    cerr << "!t" << endl;
  else
    cerr << "hoera" << endl;

  assert(t.type() == AT_APPL);
  cout << "name = " << t.function().name() << endl;
  cout << t.to_string() << endl;

  return 0;
}
