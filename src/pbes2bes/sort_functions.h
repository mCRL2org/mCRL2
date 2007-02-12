//#include "lpe/data.h"
#include "lpe/pbes.h"

//#include "lpe/sort.h"
//using namespace lpe;
//using namespace std;

lpe::sort_list get_sorts(lpe::data_variable_list v);

bool check_finite(lpe::function_list fl, lpe::sort s);

bool check_finite_list(lpe::function_list fl, lpe::sort_list s);

lpe::data_expression_list enumerate_constructors(lpe::function_list fl, lpe::sort s);

lpe::data_expression_list create_data_expression_list(lpe::function f, std::vector< lpe::data_expression_list > dess);
