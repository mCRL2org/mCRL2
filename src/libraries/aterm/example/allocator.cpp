#define ATERM_DEBUG_ALLOCATOR

#include <vector>
#include "atermpp/atermpp.h"
#include "atermpp/aterm_allocator.h"

using namespace std;
using namespace atermpp;

struct A
{
  A()
  {
    cout << "A::A()" << endl;
  }
};

void on_allocate(A* p, std::size_t num)
{
  cout << "inside on_allocate(A*)" << endl;
}

void on_deallocate(A* p, std::size_t num)
{
  cout << "inside on_deallocate(A*)" << endl;
}

void on_construct(A* p)
{
  cout << "inside on_construct(A*)" << endl;
}

void on_destroy(A* p)
{
  cout << "inside on_destroy(A*)" << endl;
}

int main()
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);

  aterm_appl t0 = make_term("f(g(x,y),h(x),f(y,z(u,v)))");
  aterm_appl t1 = make_term("f(a)");

  vector<ATerm, aterm_allocator<ATerm> > v;
  for (int i = 0; i < 10; i++)
    v.push_back(aterm_int(i));
  v.push_back(t0);
  v.push_back(t1);
  v.clear();

  // This demonstrates that aterm_allocator can be used as a custom allocator
  // for non ATerm objects. It also shows that the destroy method is being
  // called for non-pointer types by the Visual C++ compiler.
  vector<A, aterm_allocator<A> > a;
  a.push_back(A());
  a.push_back(A());
  a.push_back(A());
  a.push_back(A());

  return 0;
}
