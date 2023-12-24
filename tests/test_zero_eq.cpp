#include "PolyMgr.h"

sword::PolyMgr polyMgr;
FILE *f;

int test_ec_0(void) {
    sword::Polynomial poly1 = polyMgr.parse("(156 16 x 6) (62724 16 x 5) (17968 16 x 4) (18661 16 x 3) (43593 16 x 2) (40224 16 x 1) (13281)");
    sword::Polynomial poly2 = polyMgr.parse("(156 16 x 6) (5380 16 x 5) (1584 16 x 4) (10469 16 x 3) (27209 16 x 2) (7456 16 x 1) (13281)");
    
    return polyMgr.poly_eq(poly1, poly2, 16) ? 0 : 1;
}

int test_ec_1(void) {
    sword::Polynomial poly1 = polyMgr.parse("(156 16 x 6) (62724 16 x 5) (17968 16 x 4) (18661 16 x 3) (43593 16 x 2) (40224 16 x 1) (13281)");
    sword::Polynomial poly2 = polyMgr.parse("(156 16 x 6) (5380 16 x 5) (1583 16 x 4) (10469 16 x 3) (27209 16 x 2) (7456 16 x 1) (13281)");
    
    return polyMgr.poly_eq(poly1, poly2, 16) ? 1 : 0;
}

int test_ec_2(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 1 x1 2) (7 1 x1 1) (4 1 x1 1 2 x2 2) (4 1 x1 1 2 x2 1)");
    sword::Polynomial poly2 = polyMgr.parse("(0)");
    
    return polyMgr.poly_eq(poly1, poly2, 3) ? 0 : 1;
}

int test_ec_3(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 1 x1 2) (7 1 x1 1) (4 1 x1 1 2 x2 2) (4 1 x1 1 2 x2 1)");
    sword::Polynomial poly2 = polyMgr.parse("(0)");
    
    return polyMgr.poly_eq(poly1, poly2, 4) ? 1 : 0;
}

int test_ec_4(void) {
    sword::Polynomial poly1 = polyMgr.parse("(4 3 x 2 3 y 1)");
    sword::Polynomial poly2 = polyMgr.parse("(4 3 x 1 3 y 1)");
    
    return polyMgr.poly_eq(poly1, poly2, 3) ? 0 : 1;
}

int test_ec_5(void) {
    sword::Polynomial poly1 = polyMgr.parse("(4 3 x 2 3 y 1)");
    sword::Polynomial poly2 = polyMgr.parse("(0)");
    
    return polyMgr.poly_eq(poly1, poly2, 3) ? 1 : 0;
}

int test_ec_6(void) {
    sword::Polynomial poly1 = polyMgr.parse("(16384 12 A 4) (16384 8 B 4) (64767 12 A 2) (-64767 8 B 2) (1 12 A 1) (-1 8 B 1) (57344 12 A 2 8 B 1) (-57344 12 A 1 8 B 2)");
    sword::Polynomial poly2 = polyMgr.parse("(24576 12 A 2 8 B 1) (15615 12 A 2) (8192 12 A 1 8 B 2) (32768 12 A 1 8 B 1) (1 12 A 1) (17153 8 B 2) (65535 8 B 1)");
    
    return polyMgr.poly_eq(poly1, poly2, 16) ? 0 : 1;
}

int test_ec_7(void) {
    sword::Polynomial poly1 = polyMgr.parse("(16384 12 A 4) (16384 8 B 4) (64767 12 A 2) (-64767 8 B 2) (1 12 A 1) (-1 8 B 1) (57344 12 A 2 8 B 1) (-57344 12 A 1 8 B 2)");
    sword::Polynomial poly2 = polyMgr.parse("(24576 12 A 2 8 B 1) (15615 12 A 2) (8191 12 A 1 8 B 2) (32768 12 A 1 8 B 1) (1 12 A 1) (17153 8 B 2) (65535 8 B 1)");
    
    return polyMgr.poly_eq(poly1, poly2, 16) ? 1 : 0;
}

int test_ec_8(void) {
    sword::Polynomial poly1 = polyMgr.parse("(16384 12 A 4) (16384 8 B 4) (64767 12 A 2) (-64767 8 B 2) (1 12 A 1) (-1 8 B 1) (57344 12 A 2 8 B 1) (-57344 12 A 1 8 B 2)");
    sword::Polynomial poly2 = polyMgr.parse("(24576 12 A 2 8 B 1) (15615 12 A 2) (8192 12 A 1 8 B 2) (32768 12 A 1 8 B 1) (1 12 A 1) (17153 8 B 2) (65535 8 B 1)");
    
    return polyMgr.poly_eq(poly1, poly2, 14) ? 0 : 1;
}

int test_ec_9(void) {
    sword::Polynomial poly1 = polyMgr.parse("(16384 12 A 4) (16384 8 B 4) (64767 12 A 2) (-64767 8 B 2) (1 12 A 1) (-1 8 B 1) (57344 12 A 2 8 B 1) (-57344 12 A 1 8 B 2)");
    sword::Polynomial poly2 = polyMgr.parse("(24576 12 A 2 8 B 1) (15615 12 A 2) (8192 12 A 1 8 B 2) (32768 12 A 1 8 B 1) (1 12 A 1) (17153 8 B 2) (65535 8 B 1)");
    
    return polyMgr.poly_eq(poly1, poly2, 12) ? 0 : 1;
}

int test_ec_10(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 8 x 10) (211 8 x 9) (102 8 x 8) (22 8 x 7) (41 8 x 6) (243 8 x 5) (224 8 x 4) (36 8 x 3) (16 8 x 2) (128 8 x 1)");
    sword::Polynomial poly2 = polyMgr.parse("(0)");
    
    return polyMgr.poly_eq(poly1, poly2, 8) ? 0 : 1;
}

int test_ec_11(void) {
    sword::Polynomial poly1 = polyMgr.parse("(1 8 x 10) (211 8 x 9) (101 8 x 8) (22 8 x 7) (41 8 x 6) (243 8 x 5) (224 8 x 4) (36 8 x 3) (16 8 x 2) (128 8 x 1)");
    sword::Polynomial poly2 = polyMgr.parse("(0)");
    
    return polyMgr.poly_eq(poly1, poly2, 8) ? 1 : 0;
}

int main(void) {
    return 0 | test_ec_0() | test_ec_1() | test_ec_2() | test_ec_3() | test_ec_4() | test_ec_5() | test_ec_6() | test_ec_7() | test_ec_8() | test_ec_9() | test_ec_10() | test_ec_11();
}