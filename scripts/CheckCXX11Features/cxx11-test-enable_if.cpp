#include <iostream>
#include <type_traits>

template <typename T>
struct is_int
{
  static const bool value = false;
};

template <>
struct is_int<int>
{
  static const bool value = true;
};

template<class T>
bool is_integer(T x, typename std::enable_if<is_int<T>::value, T>::type* = 0)
{
  return true;
};

template<class T>
bool is_integer(T x, typename std::enable_if<!is_int<T>::value, T>::type* = 0)
{
  return false;
};

int main()
{
  return is_integer(10) && !is_integer(11.0) ? 0 : 1;
}
