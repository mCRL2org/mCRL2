//#include "mcrl2/data/data.h"
#include "mcrl2/pbes/pbes.h"

//#include "mcrl2/data/sort.h"
//using namespace lps;
//using namespace std;

lps::sort_list get_sorts(lps::data_variable_list v);

bool check_finite(lps::function_list fl, lps::sort s);

bool check_finite_list(lps::function_list fl, lps::sort_list s);

lps::data_expression_list enumerate_constructors(lps::function_list fl, lps::sort s);

lps::data_expression_list create_data_expression_list(lps::function f, std::vector< lps::data_expression_list > dess);
