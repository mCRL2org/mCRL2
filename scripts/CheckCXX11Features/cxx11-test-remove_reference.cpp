#include <iostream> // std::cout
#include <type_traits> // std::is_same
 
template <typename T>
struct increment
{
  typedef typename std::remove_reference<T>::type value_t;
  void direct(T x)
  {
    ++x;
  }

  void byvalue(value_t x)
  {
    ++x;
  }
};
 
int main() {
  std::cout << std::boolalpha;
  increment<int&> i;
  int t = 0;
  std::cout << "t = " << t << std::endl;
  i.direct(t);
  std::cout << "t = " << t << std::endl;
  i.byvalue(t);
  std::cout << "t = " << t << std::endl;
  return 0; 
}
