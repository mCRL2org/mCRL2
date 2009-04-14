//[ term_initialization
int main(int argc, char **argv)
{
  /*<< The ATerm Library must be initialized with a dummy term that is declared at the beginning of the `main` function. There is no known way to circumvent this.>>*/
  MCRL2_ATERM_INIT(argc, argv)
}
//]
