#include <functional>

class X
{
private:
  int m_i;
public:
  X(int i) : m_i(i) { }
  int get_sum(int j) { return m_i + j; }
};

int main()
{
  X x(1);
  return std::bind(&X::get_sum, &x, -1)();
}
