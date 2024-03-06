#include "Rewrite.h"

#include <iostream>

int main(int argc, char *argv[]) {
    sword::RWManager man;
    man.createWord("a", 3, 3);
    printf("\n");
    man.createWord("b", 3, 3);
    printf("\n");
    // man.createWord("c", 3, 3);

    man.parsePoly("np3_01.poly");
    
    return 0;
}
