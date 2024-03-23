#include "Rewrite.h"

#include <iostream>

int main(int argc, char *argv[]) {
    sword::RWManager man;

    if (argc != 2) {
        printf("usage: ./%s <filepath>\n", argv[0]);
        return 0;
    }

    man.parsePoly(argv[1]);
    printf("(check-sat)\n");
    printf("(get-model)\n");
    
    return 0;
}
