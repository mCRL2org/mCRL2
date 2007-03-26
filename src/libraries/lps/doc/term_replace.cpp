//[ term_replace
lps::sort D("D");
data_variable v("d1", D);
cout << "v  = " << pp(v) << " " << v.to_string() << endl;

data_variable w("YES", D);
data_assignment a(v, w);
cout << "a = " << pp(a) << " " << a.to_string() << endl;

data_expression_list d0 = spec.initial_process().state();
cout << "d0 = " << pp(d0) << " " << d0.to_string() << endl;

data_expression_list d1 = d0.substitute(a);
cout << "d1 = " << pp(d1) << " " << d1.to_string() << endl;

data_assignment_list aa;
aa = push_front(aa, a);
aa = push_front(aa, a);
data_expression_list d2 = d0.substitute(assignment_list_substitution(aa));
//]
