//[ term_initialization
int main()
{
  /*<< The ATerm Library must be initialized with a dummy term that is declared at the beginning of the `main` function. There is no known way to circumvent this.>>*/
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);

  /*<< The call to `gsEnableConstructorFunctions` is used for initialization of some specific terms. This function must be called after initialization of the ATerm library.>>*/
  gsEnableConstructorFunctions();
}
//]
