#include "PolyMgr.h"

#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
    sword::PolyMgr polyMgr;

    if (argc < 4) {
        printf("Usage: <func_spec> <func_impl> <out_bits>\n");
        return 0;
    }

    std::fstream spec;
    spec.open(argv[1], std::ios::in);
    if (!spec.is_open()) {
        printf("Unable to open \"%s\"\n", argv[1]);
        return 0;
    }
    std::fstream impl;
    impl.open(argv[2], std::ios::in);
    if (!impl.is_open()) {
        printf("Unable to open \"%s\"\n", argv[2]);
        return 0;
    }

    std::string func_spec;
    std::getline(spec, func_spec);
    std::string func_impl;
    std::getline(impl, func_impl);

    sword::Polynomial poly1 = polyMgr.parse(func_spec);
    sword::Polynomial poly2 = polyMgr.parse(func_impl);
    print_poly(poly1);
    print_poly(poly2);

    int eq = polyMgr.poly_eq(poly1, poly2, atoi(argv[3]));
    printf("poly1 and poly2 are %s\n", eq ? "Equivalence" : "Non-Equivalence");

    spec.close();
    impl.close();

    if (argc >= 5) {
        std::fstream result;
        result.open(argv[4], std::ios::out);
        if (result.is_open()) {
            if (eq) result << "Equivalence";
            else    result << "Non-Equivalence";
            result.close();
        }
    }

    return 0;
}