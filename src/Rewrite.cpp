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
        coeff_bit[i] = name + "_coeff" + "_" + std::to_string(i) + "";
    }
    PI(coeff_bit, outbit);
    std::string coeff = name + "_coeff";
    // std::cout << "(declare-const " << coeff << " (_ BitVec " << outbit << "))" << std::endl;
    std::cout << "(declare-const " << coeff << " Int)" << std::endl;

    std::vector<std::string> neg(inbit);
    for (int i = 0; i < inbit; ++i) {
        neg[i] = name + "_neg" + "_" + std::to_string(i) + "";
    }
    PB(neg);

    std::vector<std::vector<std::string> > bit_coeff_bit(inbit, std::vector<std::string>(outbit));
    for (int i = 0; i < inbit; ++i) { // coeff
        for (int j = 0; j < outbit; ++j) { // bit of coeff
            bit_coeff_bit[i][j] = name + "_bit_" + std::to_string(i) + "_coeff_" + std::to_string(j) + "";
        }
    }
    for (auto c : bit_coeff_bit) {
        PB(c);
    }
    std::vector<std::string> bit_coeff(outbit);
    for (int i = 0; i < outbit; ++i) {
        bit_coeff[i] = name + "_bit" + "_" + std::to_string(i) + ""; 
    }
    PB(bit_coeff);

    // constraint
    for (int i = 0; i < inbit; ++i) {
        std::string poly_coeff = (outbit > 1) ? "(+ " : "";
        for (int j = 0; j < outbit; ++j) {
            int power = 1 << j;
            std::string poly_new_term;
            if (power != 1)
                poly_new_term = std::string("(* ") + std::to_string(power) + " " + bit_coeff_bit[i][j] + ")";
            else
                poly_new_term = bit_coeff_bit[i][j];
            poly_coeff = poly_coeff + poly_new_term + " ";
        }
        poly_coeff += (outbit > 1) ? ")" : "";

        std::string beta = name + "_beta_" + std::to_string(i);
        printf("(declare-const %s Int)\n", beta.c_str());
        printf("(assert (= %s %s))\n", beta.c_str(), poly_coeff.c_str());
        std::string constraint = std::string("(- (* ") + "(- 1 " + neg[i] + ")" + " " + beta + ") (* " + neg[i] + " " + beta + "))";
        constraint = std::string("(= ") + coeff_bit[i] + " " + constraint + ")";
        constraint = std::string("(assert ") + constraint + ")";
        std::cout << constraint << std::endl;
    }

    std::string constraint = "(+ ";
    for (int i = 0; i < inbit; ++i) {
        std::string beta = name + "_beta_" + std::to_string(i);
        constraint = constraint + "(* " + neg[i] + " " + beta + ") ";
    }

    for (int j = 0; j < outbit; ++j) {
        int power = 1 << j;
        std::string poly_new_term;
        if (power != 1)
            poly_new_term = std::string("(* ") + std::to_string(power) + " " + bit_coeff[j] + ")";
        else
            poly_new_term = bit_coeff[j];
        constraint = constraint + " " + poly_new_term;
    }
    constraint += ")";
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
        poly_str = poly_str + "(1 " + std::to_string(1) + " " + name + "_coeff_" + std::to_string(i) + " 1 " + std::to_string(1) + " " + name + "_" + std::to_string(i) + " 1)";
        if (i < inbit)
            poly_str += " ";
    }
    poly_str = poly_str + "(1 " + std::to_string(1) + " " + name + "_coeff" + " 1)"; 
    // std::cout << "|" << poly_str << "|" << std::endl;
    Polynomial poly = poly_man.parse(poly_str);
    poly_pool[name] = poly_vec.size();
    poly_vec.push_back(poly);
}

void RWManager::parsePoly(std::string filepath) {
    std::ifstream f(filepath);
    std::string line;

    int outbit = 8;

    // Read number of variable
    std::getline(f, line);
    int num_variable = std::stoi(line);
    // Read variable
    std::vector<std::pair<std::string, int> > v;
    for (int i = 0; i < num_variable; ++i) {
        std::getline(f, line);
        std::string name = line.substr(0, line.find(" "));
        int bits = std::stoi(line.substr(line.find(" ") + 1));
        v.push_back({name, bits});
    }

    // Generate all vars
    std::unordered_map<std::string, int> var_bit;
    std::vector<std::string> vars;
    for (auto e : v) {

        this->createWord(std::get<0>(e), std::get<1>(e), outbit);
        printf("\n");

        var_bit[std::get<0>(e)] = std::get<1>(e);
        for (int i = 0; i < std::get<1>(e); ++i) {
            std::string var = std::get<0>(e) + "_" + std::to_string(i);
            vars.push_back(var);
        }
    }

    // Read support information
    for (int i = 0; i < num_variable; ++i) {
        std::string name = std::get<0>(v[i]);
        int bits = std::get<1>(v[i]);
        std::vector<std::set<std::string> > order;
        // prepare set
        std::set<std::string> bucket;
        for (int j = 0; j < bits; ++j) {
            bucket.insert(name + "_" + std::to_string(j));
        }
        order.push_back(bucket);
        int cover_bits = 0;

        for (int j = 0; j < bits; ++j) {
            std::getline(f, line);
            std::stringstream ss(line);
            int num;
            std::string current, include;
            ss >> current >> num;
            cover_bits += num > 0;

            // Find bucket according to current
            int current_index;
            for (current_index = 0; current_index < order.size(); ++current_index) {
                if (order[current_index].count(current)) {
                    break;
                }
            }
            auto &cbucket = order[current_index];

            // printf("%s[%d]: ", current.c_str(), num);

            std::set<std::string> nbucket;
            for (int k = 0; k < num; ++k) {
                ss >> include;
                // printf("%s ", include.c_str());

                if (cbucket.count(include)) {
                    cbucket.erase(include);
                    nbucket.insert(include);
                }
            }

            if (!nbucket.empty()) {
                if (current_index != order.size() - 1) {
                    order[current_index+1].insert(nbucket.begin(), nbucket.end());
                } else {
                    order.push_back(nbucket);
                }
            }

            // printf("\n");
        }
        // for (int i = 0; i < order.size(); ++i) {
        //     for (auto b : order[i]) {
        //         printf("%s ", b.c_str());
        //     }
        //     printf("\n");
        // }

        int uncertain_bits = bits - cover_bits;

        int current_bit_start = 0;
        for (int i = 0; i < order.size(); ++i) {
            std::set<std::string> bucket = order[i];
            int current_bit_end = current_bit_start + bucket.size() - 1;
            // printf("%d %d\n", current_bit_start, current_bit_end);
            for (auto b : bucket) {
                b = b.substr(b.find("_")+1);

                for (int forbidden = 0; forbidden < bits; ++forbidden) {
                    if (forbidden >= current_bit_start && forbidden <= current_bit_end + uncertain_bits) continue;
                    printf("(assert (not %s_bit_%s_coeff_%d))\n", name.c_str(), b.c_str(), forbidden);
                }
            }

            current_bit_start = current_bit_end + 1;
        }
    }

    // Get Terms
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

    for (auto e : v) {
        term_set.insert({std::get<0>(e)});
    }

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
        printf("(assert (and (>= s_%d 0) (< s_%d %d)))\n", s_n, s_n, (1 << outbit));
        printf("; s_%d * ");
        std::string s = "(1 " + std::to_string(1) + " s_" + std::to_string(s_n++) + " 1)";
        Polynomial poly_term = poly_man.parse(s);
        // PRINT_V(term);
        for (auto t : term) {
            printf("%s * ", t.c_str());
            Polynomial p = getWord(t);
            poly_term = poly_man.poly_mul(poly_term, p);
        }
        printf("\n", poly_term.str().c_str());
        poly_assume = poly_man.poly_add(poly_assume, poly_term);
    }
    // print_poly(poly_assume);
    Polynomial poly_remain = poly_assume;

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
            std::string var = (var_pos == std::string::npos) ? term : term.substr(0, var_pos);
            poly_str = poly_str + std::to_string(1) + " ";
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

            poly_remain = poly_man.poly_sub(poly_remain, coeff);
            continue;
        }
        // print_poly(poly_assume);
        Polynomial p = poly_man.parse(poly_str);
        // print_poly(p);
        Polynomial coeff;
        poly_man.poly_partial_div(coeff, poly_assume, p);
        // print_poly(coeff);
        Polynomial poly_sub;
        for (int i = 0; i < coeff.length(); ++i) {
            Monomial mono = coeff[i];
            // print_mono(mono);
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

        Polynomial erase_poly = poly_man.poly_mul(p, coeff);
        poly_remain = poly_man.poly_sub(poly_remain, erase_poly);

        // print_poly(poly_sub);
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
    }

    // print_poly(poly_remain);

    std::vector<Polynomial> all_comb;
    for (auto term : term_set) {
        std::vector<std::string> term_vec(term.begin(), term.end());

        if (term_vec.size() == 1) {
            std::string var = term_vec[0];
            for (int i = 0; i < var_bit[var]; ++i) {
                std::string var_expand = "(1 1 " + var + "_" + std::to_string(i) + " 1)";
                Polynomial var_poly = poly_man.parse(var_expand);
                // print_poly(var_poly);
                all_comb.push_back(var_poly);
            }
        } else {
            std::vector<Polynomial> partial_comb;
            std::string var1 = term_vec[0];
            std::string var2 = term_vec[1];
            for (int i = 0; i < var_bit[var1]; ++i) {
                std::string var1_expand = "(1 1 " + var1 + "_" + std::to_string(i) + " 1)";
                Polynomial var1_poly = poly_man.parse(var1_expand);
                for (int j = 0; j < var_bit[var2]; ++j) {
                    std::string var2_expand = "(1 1 " + var2 + "_" + std::to_string(j) + " 1)";
                    Polynomial var2_poly = poly_man.parse(var2_expand);
                    Polynomial var_poly = poly_man.poly_mul(var1_poly, var2_poly);
                    // print_poly(var_poly);
                    partial_comb.push_back(var_poly);
                }
            }

            for (int i = 2; i < term_vec.size(); ++i) {
                std::vector<Polynomial> partial_comb_updated;
                std::string var1 = term_vec[i];
                std::string var1_expand = "(1 1 " + var1 + "_" + std::to_string(i) + " 1)";
                Polynomial var1_poly = poly_man.parse(var1_expand);
                for (int j = 0; j < partial_comb.size(); ++j) {
                    Polynomial var_poly = poly_man.poly_mul(var1_poly, partial_comb[j]);
                    partial_comb_updated.push_back(var_poly);
                }
                partial_comb = partial_comb_updated;
            }
            all_comb.insert(all_comb.end(), partial_comb.begin(), partial_comb.end());
        }
    }
    for (int i = 0; i < all_comb.size(); ++i) {
        // print_poly(all_comb[i]);

        Polynomial q;
        poly_man.poly_partial_div(q, poly_remain, all_comb[i]);
        if (q.empty()) continue;

        Polynomial poly_sub;
        for (int i = 0; i < q.length(); ++i) {
            Monomial mono = q[i];
            // print_mono(mono);
            std::string mono_str = mono.str();
            for (auto var : vars) {
                if (mono_str.find(var) != std::string::npos) {
                    Polynomial p = poly_man.getPoly({mono});
                    poly_sub = poly_man.poly_add(poly_sub, p);
                    break;
                }
            }
        }
        q = poly_man.poly_sub(q, poly_sub);
        if (q.empty()) continue;
        // print_poly(q);

        printf("(assert (= %d (mod (", 0);
        if (q.length() > 1) {
            printf("+ ");
            for (int i = 0; i < q.length(); ++i) {
                Monomial mono = q[i];
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
            std::set<Var> vars = q.vars();
            for (auto var : vars) {
                printf("%s ", var.name());
            }
            printf(")");
        }
        printf(" %d)))\n", (1 << outbit));

    }
}

Polynomial RWManager::getWord(std::string name) {
    return poly_vec[poly_pool[name]];
}

} // namespace sword
