#include "PolyMgr.h"

sword::PolyMgr polyMgr;
FILE *f;

int test_sub_0(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 3 x 1)");
    sword::Polynomial poly2 = polyMgr.parse("(1 3 x 1)");

    sword::Polynomial poly_sub = polyMgr.poly_sub(poly1, poly2);
    std::string str = poly_sub.str();
    
    return str == "" ? 0 : 1;
}

int test_sub_1(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 3 x 1)");
    sword::Polynomial poly2 = polyMgr.parse("(-2 3 x 1)");

    sword::Polynomial poly_sub = polyMgr.poly_sub(poly1, poly2);
    std::string str = poly_sub.str();
    
    return str == " +3*x^1" ? 0 : 1;
}

int test_sub_2(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 3 x 1)");
    sword::Polynomial poly2 = polyMgr.parse("(1 3 y 1) (-2 3 x 1)");

    sword::Polynomial poly_sub = polyMgr.poly_sub(poly1, poly2);
    std::string str = poly_sub.str();
    
    return str == " +3*x^1 -1*y^1" || str == " -1*y^1 +3*x^1" ? 0 : 1;
}

int test_sub_3(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 3 x 1)");
    sword::Polynomial poly2 = polyMgr.parse("(1)");

    sword::Polynomial poly_sub = polyMgr.poly_sub(poly1, poly2);
    std::string str = poly_sub.str();
    
    return str == " +1*x^1 -1*CONST^0" ? 0 : 1;
}

int main(void) {
    return 0 | test_sub_0() | test_sub_1() | test_sub_2() | test_sub_3();
}