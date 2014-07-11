#include <iostream> 
#include <utility>
#include <type_traits>

class Int
{
private:
  int m_int;
public:
  typedef std::add_lvalue_reference<Int>::type ref_t;
  typedef std::add_rvalue_reference<Int>::type move_t;

  Int(int i) : m_int(i) { std::cout << "[const] "; }
  Int(move_t i) : m_int(i.m_int) { std::cout << "[move] "; }
  Int(ref_t i) : m_int(i.m_int) { std::cout << "[copy] "; }
};
 
int main() {
  Int i1(1);
  std::cout << std::endl;
  Int i2(i1);
  std::cout << std::endl;
  Int i3(std::move(i1));
  std::cout << std::endl;
  return 0; 
}
