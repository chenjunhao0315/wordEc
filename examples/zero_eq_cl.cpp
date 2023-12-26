#include "PolyMgr.h"

#include <iostream>
#include <sstream>

sword::PolyMgr polyMgr;
int unsupport = 0;

sword::Polynomial rec(std::stringstream &ss, int &out_bits) {
    if (ss.tellg() == ss.str().length()) return sword::Polynomial();

    std::string op;
    ss >> op;
    char checkpoint;

    sword::Polynomial poly1, poly2;

    if (op == "var") {
        std::string name;
        int bit;

        ss >> name >> bit;
        std::string poly_str = "(1 " + std::to_string(bit) + " " + name + " 1)";

        ss >> checkpoint;
        if (checkpoint == ')') {
            return polyMgr.parse(poly_str);
        } else {
            printf("[Var] Syntax Error!\n");
        }
    }

    ss >> checkpoint;
    if (checkpoint == '(') {
        poly1 = rec(ss, out_bits);
    } else {
        ss.putback(checkpoint);
        int constant;
        ss >> constant;
        std::string poly_str = "(" + std::to_string(constant) + ")";
        poly1 = polyMgr.parse(poly_str);
    }

    ss >> checkpoint;
    if (checkpoint == '(') {
        poly2 = rec(ss, out_bits);
    } else {
        ss.putback(checkpoint);
        int constant;
        ss >> constant;
        std::string poly_str = "(" + std::to_string(constant) + ")";
        poly2 = polyMgr.parse(poly_str);
    }

    // output bit
    ss >> out_bits;
    ss >> checkpoint;
    if (checkpoint != ')') {
        printf("[Poly2] Syntax Error!\n");
    }

    if (op == "+") {
        sword::Polynomial poly_add = polyMgr.poly_add(poly1, poly2);
        return poly_add;
    } else if (op == "*") {
        sword::Polynomial poly_mul = polyMgr.poly_mul(poly1, poly2);
        return poly_mul;
    } else if (op == "-") {
        sword::Polynomial poly_sub = polyMgr.poly_sub(poly1, poly2);
        return poly_sub;
    } else {
        unsupport = 1;
    }

    return sword::Polynomial();
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: <func_spec> <func_impl>\n");
        return 0;
    }

    std::stringstream ss1(argv[1]);
    int constant_1 = 0;
    int out_bits_1 = -1;
    sword::Polynomial poly1;

    char checkpoint;
    ss1 >> checkpoint;
    if (checkpoint == '(') {
        poly1 = rec(ss1, out_bits_1);
    } else if (isalpha(checkpoint)) {
        return 0;
    } else {
        if (atoi(argv[1]) == 0) {
            poly1 = polyMgr.parse("(0)");
        } else {
            poly1 = polyMgr.getConst(atoi(argv[1]));
        }
        constant_1 = atoi(argv[1]);
        out_bits_1 = 0;
    }

    std::stringstream ss2(argv[2]);
    int constant_2 = -1;
    int out_bits_2 = -1;
    sword::Polynomial poly2;

    ss2 >> checkpoint;
    if (checkpoint == '(') {
        poly2 = rec(ss2, out_bits_2);
    } else if (isalpha(checkpoint)) {
        return 0;
    } else {
        if (atoi(argv[2]) == 0) {
            poly2 = polyMgr.parse("(0)");
        } else {
            poly2 = polyMgr.getConst(atoi(argv[2]));
        }
        constant_2 = atoi(argv[2]);
        out_bits_2 = 0;
    }

    if (out_bits_1 == 0 && out_bits_2 == 0) {
        // printf("poly1 and poly2 are %s\n", constant_1 == constant_2 ? "Equivalence" : "Non-Equivalence");
        return constant_1 == constant_2;
    }

    if (out_bits_1 == -1) {
        out_bits_1 = poly1.highest_bits();
    }
    if (out_bits_2 == -1) {
        out_bits_2 = poly2.highest_bits();
    }

    if (out_bits_1 != out_bits_2 && !(out_bits_1 == 0 || out_bits_2 == 0)) {
        return 0;
    }
    if (unsupport) {
        return 0;
    }

    if (poly2.highest_order() > poly1.highest_order()) {
        sword::Polynomial poly_temp = poly1;
        poly1 = poly2;
        poly2 = poly_temp;
        int temp = out_bits_1;
        out_bits_1 = out_bits_2;
        out_bits_2 = temp;
    }

    // print_poly(poly1);
    // print_poly(poly2);

    int eq = polyMgr.poly_eq(poly1, poly2, out_bits_1);
    // printf("poly1 and poly2 are %s\n", eq ? "Equivalence" : "Non-Equivalence");

    return eq;
}