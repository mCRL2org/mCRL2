class Foo {
  public:
    Foo(int i) { }
    Foo() = delete;
    Foo(const Foo&) = delete;
    Foo& operator=(const Foo&) = delete;
};

int main()
{
  Foo x(0);
  return 0;
}
