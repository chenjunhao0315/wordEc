#include "Rewrite.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <sstream>

#define PRINT(v) do { for (auto e : v) std::cout << e << std::endl; } while (0)
#define PRINT_V(v) do { for (auto e : v) std::cout << e << ", "; std::cout << std::endl; } while (0)
// #define PV(v) do { for (auto e : v) std::cout << e << ", "; std::cout << std::endl; } while (0)
#define PB(v) do { for (auto e : v) std::cout << "(declare-const " << e << " Bool)" << std::endl; } while (0)
// #define PI(v, bit) do { for (auto e : v) std::cout << "(declare-const " << e << " (_ BitVec " << bit << "))" << std::endl; } while (0)
#define PI(v, bit) do { for (auto e : v) std::cout << "(declare-const " << e << " Int)" << std::endl; } while (0)

namespace sword {

void RWManager::createWord(std::string name, int inbit, int outbit) {
    std::vector<std::string> coeff_bit(inbit);
    for (int i = 0; i < inbit; ++i) {
        coeff_bit[i] = name + "_c" + "_" + std::to_string(i) + "";
    }
    PI(coeff_bit, outbit);
    std::string coeff = name + "_c";
    // std::cout << "(declare-const " << coeff << " (_ BitVec " << outbit << "))" << std::endl;
    std::cout << "(declare-const " << coeff << " Int)" << std::endl;

    std::vector<std::string> neg(inbit);
    for (int i = 0; i < inbit; ++i) {
        neg[i] = name + "_n" + "_" + std::to_string(i) + "";
    }
    PB(neg);

    std::vector<std::vector<std::string> > bit_coeff_bit(inbit, std::vector<std::string>(outbit));
    for (int i = 0; i < inbit; ++i) { // coeff
        for (int j = 0; j < outbit; ++j) { // bit of coeff
            bit_coeff_bit[i][j] = name + "_b" + "^c_" + std::to_string(i) + "" + "_" + std::to_string(j) + "";
        }
    }
    for (auto c : bit_coeff_bit) {
        PB(c);
    }
    std::vector<std::string> bit_coeff(outbit);
    for (int i = 0; i < outbit; ++i) {
        bit_coeff[i] = name + "_b" + "_" + std::to_string(i) + ""; 
    }
    PB(bit_coeff);

    // constraint
    for (int i = 0; i < inbit; ++i) {
        std::string poly_coeff = bit_coeff_bit[i][0];
        for (int j = 1; j < outbit; ++j) {
            int power = 1 << j;
            std::string poly_new_term = "(* " + std::to_string(power) + " " + bit_coeff_bit[i][j] + ")";
            poly_coeff = "(+ " + poly_coeff + " " + poly_new_term + ")";
        }
        std::string constraint = std::string("(- (* ") + "(- 1 " + neg[i] + ")" + " " + poly_coeff + ") (* " + neg[i] + " " + poly_coeff + "))";
        constraint = std::string("(= ") + coeff_bit[i] + " " + constraint + ")";
        constraint = std::string("(assert ") + constraint + ")";
        std::cout << constraint << std::endl;
    }
    std::string constraint = std::string("(* ") + bit_coeff_bit[0][0] + " " + neg[0] + ")";
    for (int i = 0; i < inbit; ++i) {
        for (int j = 0; j < outbit; ++j) {
            if (i == 0 && j == 0) continue;
            int power = 1 << j;
            std::string poly_new_term = "(* " + bit_coeff_bit[i][j] + " " + neg[i] + ")";
            if (power != 1)
                poly_new_term = "(* " + std::to_string(power) + " " + poly_new_term + ")";
            constraint = "(+ " + constraint + " " + poly_new_term + ")";
        }
    }
    for (int j = 0; j < outbit; ++j) {
        int power = 1 << j;
        std::string poly_new_term;
        if (power != 1)
            poly_new_term = std::string("(* ") + std::to_string(power) + " " + bit_coeff[j] + ")";
        else
            poly_new_term = bit_coeff[j];
        constraint = "(+ " + constraint + " " + poly_new_term + ")";
    }
    constraint = std::string("(= ") + coeff + " " + constraint + ")";
    constraint = std::string("(assert ") + constraint + ")";
    std::cout << constraint << std::endl;

    for (int i = 0; i < outbit; ++i) {
        std::string constraint = std::string("(assert ((_ at-most 1) ") + bit_coeff[i];
        for (int j = 0; j < inbit; ++j) {
            constraint = constraint + " " + bit_coeff_bit[j][i];
        }
        constraint = constraint + "))";
        std::cout << constraint << std::endl;
    }

    std::string poly_str;
    for (int i = 0; i < inbit; ++i) {
        poly_str = poly_str + "(1 " + std::to_string(outbit) + " " + name + "_c_" + std::to_string(i) + " 1 " + std::to_string(inbit) + " " + name + "_" + std::to_string(i) + " 1)";
        if (i < inbit)
            poly_str += " ";
    }
    poly_str = poly_str + "(1 " + std::to_string(outbit) + " " + name + "_c" + " 1)"; 
    // std::cout << "|" << poly_str << "|" << std::endl;
    Polynomial poly = poly_man.parse(poly_str);
    poly_pool[name] = poly_vec.size();
    poly_vec.push_back(poly);
}

void RWManager::parsePoly(std::string filepath) {
    int inbit = 3;
    int outbit = 3;
    std::vector<std::string> vars = {"a_0", "a_1", "a_2", "b_0", "b_1", "b_2"};
    std::ifstream f(filepath);
    std::string line;
    std::vector<std::string> terms;
    std::set<std::set<std::string> > term_set;

    while (std::getline(f, line)) {
        terms.emplace_back(line);
    }

    f.close();

    std::sort(terms.begin(), terms.end(), [](std::string a, std::string b) {
        int na = std::count(a.begin(), a.end(), '*');
        int nb = std::count(b.begin(), b.end(), '*');
        return na > nb;
    });

    for (auto term : terms) {
        std::set<std::string> var_set;
        do {
            int pos = term.find("*");
            if (pos == std::string::npos) break;
            term = term.substr(pos+1);
            std::string var_name = term.substr(0, term.find("_"));
            var_set.insert(var_name);
        } while (1);
        if (var_set.size())
            term_set.insert(var_set);
    }
    Polynomial poly_assume;
    int s_n = 0;
    for (auto term : term_set) {
        printf("(declare-const s_%d Int)\n", s_n);
        std::string s = "(1 " + std::to_string(outbit) + " s_" + std::to_string(s_n++) + " 1)";
        Polynomial poly_term = poly_man.parse(s);
        for (auto t : term) {
            Polynomial p = getWord(t);
            poly_term = poly_man.poly_mul(poly_term, p);
        }
        poly_assume = poly_man.poly_add(poly_assume, poly_term);
    }

    // coeff observation
    for (auto term : terms) {
        int coeff_int;
        std::stringstream ss(term);
        ss >> coeff_int;
        if (coeff_int < 0) coeff_int += (1 << outbit);
        // std::cout << coeff_int << std::endl;
        std::string poly_str = "(1 ";
        do {
            int pos = term.find("*");
            if (pos == std::string::npos) break;
            term = term.substr(pos+1);
            int var_pos = term.find("*");
            poly_str = poly_str + std::to_string(inbit) + " ";
            std::string var = (var_pos == std::string::npos) ? term : term.substr(0, var_pos);
            poly_str = poly_str + var + " 1 ";
        } while (1);
        poly_str = poly_str + ")";
        // std::cout << "|" << poly_str << "|" << std::endl;
        if (poly_str == "(1 )") {
            Polynomial poly_sub;
            for (int i = 0; i < poly_assume.length(); ++i) {
                Monomial mono = poly_assume[i];
                std::string mono_str = mono.str();
                for (auto var : vars) {
                    if (mono_str.find(var) != std::string::npos) {
                        Polynomial p = poly_man.getPoly({mono});
                        poly_sub = poly_man.poly_add(poly_sub, p);
                        break;
                    }
                }
            }
            Polynomial coeff = poly_man.poly_sub(poly_assume, poly_sub);
            // print_poly(coeff);
            printf("(assert (= %d (mod (", coeff_int);
            if (coeff.length() > 1) {
                printf("+ ");
                for (int i = 0; i < coeff.length(); ++i) {
                    Monomial mono = coeff[i];
                    printf("(* ");
                    std::set<Var> vars = mono.vars();
                    for (auto var : vars) {
                        printf("%s ", var.name());
                    }
                    printf(") ");
                }
                printf(")");
            } 
            printf(" %d)))\n", (1 << outbit));
            continue;
        }
        Polynomial p = poly_man.parse(poly_str);
        // print_poly(p);
        Polynomial coeff;
        poly_man.poly_partial_div(coeff, poly_assume, p);
        Polynomial poly_sub;
        for (int i = 0; i < coeff.length(); ++i) {
            Monomial mono = coeff[i];
            std::string mono_str = mono.str();
            for (auto var : vars) {
                if (mono_str.find(var) != std::string::npos) {
                    Polynomial p = poly_man.getPoly({mono});
                    poly_sub = poly_man.poly_add(poly_sub, p);
                    break;
                }
            }
        }
        coeff = poly_man.poly_sub(coeff, poly_sub);
        // print_poly(coeff);
        printf("(assert (= %d (mod (", coeff_int);
        if (coeff.length() > 1) {
            printf("+ ");
            for (int i = 0; i < coeff.length(); ++i) {
                Monomial mono = coeff[i];
                printf("(* ");
                std::set<Var> vars = mono.vars();
                for (auto var : vars) {
                    printf("%s ", var.name());
                }
                printf(") ");
            }
            printf(")");
        } else {
            printf("* ");
            std::set<Var> vars = coeff.vars();
            for (auto var : vars) {
                printf("%s ", var.name());
            }
            printf(")");
        }
        printf(" %d)))\n", (1 << outbit));
        std::cout << std::endl;
    }
}

Polynomial RWManager::getWord(std::string name) {
    return poly_vec[poly_pool[name]];
}

} // namespace sword
