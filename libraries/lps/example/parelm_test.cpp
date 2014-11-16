#include <string>
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parelm.h"

using namespace mcrl2;

void test_parelm(std::string spec_text)
{
  lps::specification spec1 = lps::linearise(spec_text);
  lps::specification spec2 = spec1;
  lps::parelm(spec2);
  std::cout << "<before>\n" << lps::pp(spec1.process()) << std::endl;
  std::cout << "<after>\n"  << lps::pp(spec2.process()) << std::endl;
  std::cout << "------------------------------------------------------------------------" << std::endl;
}

std::string SPEC1 =
  "% Test Case 1                            \n"
  "%                                        \n"
  "% Process parameter i should be removed. \n"
  "                                         \n"
  "act a;                                   \n"
  "                                         \n"
  "proc X(i: Nat) = a.X(i);                 \n"
  "                                         \n"
  "init X(2);                               \n"
  ;

std::string SPEC2 =
  "% Test Case 2                            \n"
  "%                                        \n"
  "% Process parameter j should be removed  \n"
  "                                         \n"
  "act a: Nat;                              \n"
  "                                         \n"
  "proc X(i,j: Nat) = a(i). X(i,j);         \n"
  "                                         \n"
  "init X(0,1);                             \n"
  ;

std::string SPEC3 =
  "% Test Case 3                               \n"
  "%                                           \n"
  "% Process parameter j should be removed     \n"
  "                                            \n"
  "act a;                                      \n"
  "                                            \n"
  "proc X(i,j: Nat)   = (i == 5) -> a. X(i,j); \n"
  "                                            \n"
  "init X(0,1);                                \n"
  ;

std::string SPEC4 =
  "% Test Case 4                           \n"
  "%                                       \n"
  "% Process parameter j should be removed \n"
  "                                        \n"
  "act a;                                  \n"
  "                                        \n"
  "proc X(i,j: Nat) = a@i.X(i,j);          \n"
  "                                        \n"
  "init X(0,4);                            \n"
  ;

std::string SPEC5 =
  "% Test Case 5                            \n"
  "%                                        \n"
  "% No process parameter should be removed \n"
  "                                         \n"
  "act a: Nat;                              \n"
  "act b;                                   \n"
  "                                         \n"
  "proc X(i,j,k: Nat) =  a(i).X(k,j,k) +    \n"
  "                         b.X(j,j,k);     \n"
  "                                         \n"
  "init X(1,2,3);                           \n"
  ;

std::string SPEC6 =
  "% Test Case 6                                                \n"
  "%                                                            \n"
  "% The following LPS is generated:                            \n"
  "%                                                            \n"
  "% proc P(s3: Pos, i,j: Nat) =                                \n"
  "%       (s3 == 1 && i < 5) ->                                \n"
  "%         act1(i) .                                          \n"
  "%         P(s3 := 1, i := i + 1, j := freevar0)              \n"
  "%     + (s3 == 1 && i == 5) ->                               \n"
  "%         act3(i) .                                          \n"
  "%         P(s3 := 2, j := i)                                 \n"
  "%     + (s3 == 2) ->                                         \n"
  "%         act2(j) .                                          \n"
  "%         P(s3 := 2, j := j + 1);                            \n"
  "%                                                            \n"
  "% var  freevar: Nat;                                         \n"
  "% init P(s3 := 1, i := 0, j := freevar);                     \n"
  "%                                                            \n"
  "% from this mcrl2 specification:                             \n"
  "                                                             \n"
  "act act1, act2, act3: Nat;                                   \n"
  "                                                             \n"
  "proc X(i: Nat)   = (i <  5) -> act1(i).X(i+1) +              \n"
  "                   (i == 5) -> act3(i).Y(i, i);              \n"
  "     Y(i,j: Nat) = act2(j).Y(i,j+1);                         \n"
  "                                                             \n"
  "init X(0);                                                   \n"
  "                                                             \n"
  "% In this LPS no process parameters can be eliminated        \n"
  "% Because all process parameters are used                    \n"
  "%                                                            \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% \n"
  "%% NOTE:                                                  %% \n"
  "%% =====                                                  %% \n"
  "%%                                                        %% \n"
  "%% Use: mcrl22lps --no-cluster $DIR$/case6.mcrl2          %% \n"
  "%%                                                        %% \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% \n"
  ;

std::string SPEC7 =
  "% Test Case 7                                                \n"
  "%                                                            \n"
  "% The following LPS is generated:                            \n"
  "%                                                            \n"
  "% act  act1,act2,act3: Nat;                                  \n"
  "%                                                            \n"
  "% proc P(i,z,j: Nat) =                                       \n"
  "%       (i == 5) ->                                          \n"
  "%         act3(i) .                                          \n"
  "%         P(z := j,j := 4)                                   \n"
  "%     + (i < 5) ->                                           \n"
  "%         act1(i) @ Nat2Real(z) .                            \n"
  "%         P(i := i + 1);                                     \n"
  "%                                                            \n"
  "% init P(i := 0, z := Pos2Nat(5), j := Pos2Nat(1));          \n"
  "%                                                            \n"
  "% from this mcrl2 specification:                             \n"
  "                                                             \n"
  "act act1, act2, act3: Nat;                                   \n"
  "                                                             \n"
  "proc X(i,z,j: Nat)   = (i <  5) -> act1(i)@z.X(i+1,z, j) +   \n"
  "                       (i == 5) -> act3(i).X(i, j, 4);       \n"
  "                                                             \n"
  "init X(0,5, 1);                                              \n"
  "                                                             \n"
  "% No process parameters are removed. z is dependent of j.    \n"
  "%                                                            \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% \n"
  "%% NOTE:                                                  %% \n"
  "%% =====                                                  %% \n"
  "%%                                                        %% \n"
  "%% Use: mcrl22lps --no-cluster $DIR$/case7.mcrl2          %% \n"
  "%%                                                        %% \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% \n"
  ;

void test_stochastic_specification()
{
  std::string text =
    "% Test Case 1                            \n"
    "%                                        \n"
    "% Process parameter i should be removed. \n"
    "                                         \n"
    "act a;                                   \n"
    "                                         \n"
    "proc X(i: Nat) = a.X(i);                 \n"
    "                                         \n"
    "init X(2);                               \n"
    ;
  lps::stochastic_specification spec1 = lps::linearise(text);
  lps::stochastic_specification spec2 = spec1;
  lps::parelm(spec2);
  std::cout << "<before>\n" << lps::pp(spec1.process()) << std::endl;
  std::cout << "<after>\n"  << lps::pp(spec2.process()) << std::endl;
}

int main(int argc, char* argv[])
{
  test_parelm(SPEC1);
  test_parelm(SPEC2);
  test_parelm(SPEC3);
  test_parelm(SPEC4);
  test_parelm(SPEC5);
  test_parelm(SPEC6);
  test_parelm(SPEC7);
  test_stochastic_specification();

  return 0;
}
