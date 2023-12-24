#include "PolyMgr.h"

sword::PolyMgr polyMgr;
FILE *f;

int test_mul_0(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 3 x 1)");
    sword::Polynomial poly2 = polyMgr.parse("(1 3 x 1)");

    sword::Polynomial poly_mul = polyMgr.poly_mul(poly1, poly2);
    std::string str = poly_mul.str();
    
    return str == " +1*x^2" ? 0 : 1;
}

int test_mul_1(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 3 x 1)");
    sword::Polynomial poly2 = polyMgr.parse("(-2 3 x 1)");

    sword::Polynomial poly_mul = polyMgr.poly_mul(poly1, poly2);
    std::string str = poly_mul.str();
    
    return str == " -2*x^2" ? 0 : 1;
}

int test_mul_2(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 3 x 1)");
    sword::Polynomial poly2 = polyMgr.parse("(1 3 y 1) (-2 3 x 1)");

    sword::Polynomial poly_mul = polyMgr.poly_mul(poly1, poly2);
    std::string str = poly_mul.str();
    
    return str == " -2*x^2 +1*x^1*y^1" || str == " -2*x^2 +1*y^1*x^1" ? 0 : 1;
}

int test_mul_3(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 3 x 1)");
    sword::Polynomial poly2 = polyMgr.parse("(1)");

    sword::Polynomial poly_mul = polyMgr.poly_mul(poly1, poly2);
    std::string str = poly_mul.str();
    
    return str == " +1*x^1" ? 0 : 1;
}

int main(void) {
    return 0 | test_mul_0() | test_mul_1() | test_mul_2() | test_mul_3();
}