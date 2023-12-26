#include <iostream>
#include <sstream>
#include <fstream>

#include "PolyMgr.h"

#define PRINT(c) std::cout << c << std::endl

sword::PolyMgr polyMgr;
int out_bits = 0;
int unsupport = 0;

sword::Polynomial rec(std::stringstream &ss) {
    if (ss.tellg() == ss.str().length()) return sword::Polynomial();

    std::string op;
    ss >> op;
    // PRINT(op);
    char checkpoint;

    sword::Polynomial poly1, poly2;

    if (op == "var") {
        std::string name;
        int bit;

        ss >> name >> bit;
        std::string poly_str = "(1 " + std::to_string(bit) + " " + name + " 1)";
        // PRINT(poly_str);

        ss >> checkpoint;
        if (checkpoint == ')') {
            return polyMgr.parse(poly_str);
        } else {
            printf("[Var] Syntax Error!\n");
        }
    }

    ss >> checkpoint;
    // PRINT(checkpoint);
    if (checkpoint == '(') {
        poly1 = rec(ss);
    } else {
        ss.putback(checkpoint);
        int constant;
        ss >> constant;
        std::string poly_str = "(" + std::to_string(constant) + ")";
        poly1 = polyMgr.parse(poly_str);
    }

    ss >> checkpoint;
    // PRINT(checkpoint);
    if (checkpoint == '(') {
        poly2 = rec(ss);
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

    // print_poly(poly1);
    // print_poly(poly2);

    if (op == "+") {
        sword::Polynomial poly_add = polyMgr.poly_add(poly1, poly2);
        // print_poly(poly_add);
        return poly_add;
    } else if (op == "*") {
        sword::Polynomial poly_mul = polyMgr.poly_mul(poly1, poly2);
        // print_poly(poly_mul);
        return poly_mul;
    } else {
        unsupport = 1;
    }

    return sword::Polynomial();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: <prefix>\n");
        return 0;
    }

    const char *prefix = argv[1];

    // printf("Input: \"%s\"\n", argv[1]);

    std::stringstream ss(prefix);
    sword::Polynomial poly;

    char checkpoint;
    ss >> checkpoint;
    // PRINT(checkpoint);
    if (checkpoint == '(') {
        poly = rec(ss);
    } else {
        poly = polyMgr.getConst(atoi(prefix));
    }

    // printf("Bits: %d ", out_bits);
    // print_poly(poly);
    sword::Polynomial const_zero = polyMgr.parse("(0)");
    int eq = (out_bits) ? polyMgr.poly_eq(poly, const_zero, out_bits) : 0;
    // printf("The poly is %s\n", eq ? "vanishing polynomial" : "normal polynomial");

    if (argc >= 3) {
        std::fstream result;
        result.open(argv[2], std::ios::out);
        if (result.is_open()) {
            if (eq) result << "Vanishing";
            else    result << "Non-Vanishing";
            result.close();
        }
    }

    return eq;
}