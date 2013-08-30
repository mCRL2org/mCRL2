#include <memory>

struct foo {
  int bar;

  bool f()
  {
    return bar == 1;
  };

  foo(int bar_)
    : bar(bar_)
  {}
};

int main(void)
{
  std::unique_ptr<foo> p1(new foo(1)); // p1 owns foo
  if (p1)
  {
    if(!p1->f())
    {
      return 1;
    }
  }

  {
    std::unique_ptr<foo> p2(std::move(p1)); // p2 now owns foo
    if(!p2->f())
    {
      return 1;
    }

    p1 = std::move(p2); // ownership back to p1
  }
  // at this point p2 has been destroyed.

  if(!p1->f())
  {
    return 1;
  }
  return 0;
}
