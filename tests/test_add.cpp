#include "PolyMgr.h"
#include "readFile.h"

#include <iostream>

sword::PolyMgr polyMgr;
FILE *f;

int test_add_0(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 3 x 1)");
    sword::Polynomial poly2 = polyMgr.parse("(-1 3 x 1)");

    sword::Polynomial poly_add = polyMgr.poly_add(poly1, poly2);
    std::string str = poly_add.str();
    
    return str == "" ? 0 : 1;
}

int test_add_1(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 3 x 1)");
    sword::Polynomial poly2 = polyMgr.parse("(-2 3 x 1)");

    sword::Polynomial poly_add = polyMgr.poly_add(poly1, poly2);
    std::string str = poly_add.str();
    
    return str == " -1*x^1" ? 0 : 1;
}

int test_add_2(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 3 x 1)");
    sword::Polynomial poly2 = polyMgr.parse("(1 3 y 1) (-2 3 x 1)");

    sword::Polynomial poly_add = polyMgr.poly_add(poly1, poly2);
    std::string str = poly_add.str();
    
    return str == " -1*x^1 +1*y^1" ? 0 : 1;
}

int test_add_3(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 3 x 1)");
    sword::Polynomial poly2 = polyMgr.parse("(1)");

    sword::Polynomial poly_add = polyMgr.poly_add(poly1, poly2);
    std::string str = poly_add.str();
    
    return str == " +1*x^1 +1*CONST^0" ? 0 : 1;
}

int main(void) {
    return 0 | test_add_0() | test_add_1() | test_add_2() || test_add_3();
}