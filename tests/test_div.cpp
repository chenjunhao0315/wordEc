#include "PolyMgr.h"
#include "readFile.h"

#include <iostream>

sword::PolyMgr polyMgr;
FILE *f;

int test_div_0(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 3 x 2) (1 3 y 2) (-2 3 x 1 3 y 1)");
    sword::Polynomial poly2 = polyMgr.parse("(1 3 x 1) (-1 3 y 1)");

    sword::Polynomial quo, rem;
    polyMgr.poly_div(quo, rem, poly1, poly2);
    std::string quo_str = quo.str();
    std::string rem_str = rem.str();
    std::string str = quo_str + "\n" + rem_str;
    
    return str == " +1*x^1 -1*y^1\n" ? 0 : 1;
}

int test_div_1(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 3 x 2) (1 3 y 2) (-2 3 x 1 3 y 1)");
    sword::Polynomial poly2 = polyMgr.parse("(1 3 x 1) (1 3 y 1)");

    sword::Polynomial quo, rem;
    polyMgr.poly_div(quo, rem, poly1, poly2);
    std::string quo_str = quo.str();
    std::string rem_str = rem.str();
    std::string str = quo_str + "\n" + rem_str;
    
    return str == " +1*x^1 +1*y^1\n -4*x^1*y^1" ? 0 : 1;
}

int test_div_2(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 1 a 1 1 c 1) (1 1 a 1 1 d 1) (1 1 b 1 1 c 1) (1 1 b 1 1 d 1) (1 1 e 1)");
    sword::Polynomial poly2 = polyMgr.parse("(1 1 a 1) (1 1 b 1)");

    sword::Polynomial quo, rem;
    polyMgr.poly_div(quo, rem, poly1, poly2);
    std::string quo_str = quo.str();
    std::string rem_str = rem.str();
    std::string str = quo_str + "\n" + rem_str;
    
    return str == " +1*c^1 +1*d^1\n +1*e^1" ? 0 : 1;
}

int test_div_3(void) {
    sword::Polynomial poly1 = polyMgr.parse("(2 3 x 1)");
    sword::Polynomial poly2 = polyMgr.parse("(1)");

    sword::Polynomial quo, rem;
    polyMgr.poly_div(quo, rem, poly1, poly2);
    std::string quo_str = quo.str();
    std::string rem_str = rem.str();
    std::string str = quo_str + "\n" + rem_str;
    
    return str == " +2*x^1\n" ? 0 : 1;
}

int main(void) {
    return 0 | test_div_0() | test_div_1() | test_div_2() | test_div_3();
}