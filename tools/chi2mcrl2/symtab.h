#define NSYMS 20000

struct symtab {
  char *name;
  double (*funcptr)();
  double value;
  double type;
} symtab[NSYMS];

struct symtab *symlook();
