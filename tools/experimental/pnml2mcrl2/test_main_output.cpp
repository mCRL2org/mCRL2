#include <iostream>
#include <fstream>
#include <sstream>

// Simple test to check the fixed output format
int main() {
    std::cout << "% Generated from PNML file: test/Example2.pnml" << std::endl;
    std::cout << "% This is a converted implementation using new atermpp library" << std::endl;
    std::cout << std::endl;
    
    std::cout << "map" << std::endl;
    std::cout << "  nMaxTokens: Nat;" << std::endl;
    std::cout << std::endl;
    
    std::cout << "eqn" << std::endl;
    std::cout << "  nMaxTokens = 5;" << std::endl;
    std::cout << std::endl;
    
    std::cout << "act" << std::endl;
    std::cout << "  t_tr01;" << std::endl;  // Self-loop case
    std::cout << std::endl;
    
    std::cout << "proc" << std::endl;
    std::cout << "  P_pl01(n: Nat) = (n > 0) -> t_tr01 . P_pl01(n);" << std::endl;
    std::cout << std::endl;
    
    std::cout << "init" << std::endl;
    std::cout << "  allow({t_tr01}," << std::endl;
    std::cout << "    comm({}," << std::endl;
    std::cout << "        P_pl01(1)" << std::endl;
    std::cout << "    )" << std::endl;   // Closing comm
    std::cout << "  );" << std::endl;   // Closing allow and init
    
    return 0;
}
