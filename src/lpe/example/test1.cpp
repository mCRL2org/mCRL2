#include <string>
#include "atermpp/aterm.h"
#include "mcrl2/sort.h"

using namespace std;
using namespace atermpp;

int main()
{
  string filename("data/abp_b.lpe");
  aterm_appl t = read_from_named_file(filename).to_appl();
  if (!t)
    cerr << "!t" << endl;
  else
    cerr << "hoera" << endl;

  assert(t.type() == AT_APPL);
  cout << "name = " << t.function().name() << endl;
  cin.get();
  cout << t.to_string() << endl;
  cin.get();

  return 0;
}
