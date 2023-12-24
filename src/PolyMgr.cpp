#include "PolyMgr.h"

#include <iostream>
#include <sstream>

namespace sword {

bool PolyMgr::is_const(Term& term) const {
    return this->const_term.eq(term);
}

bool PolyMgr::is_const(Monomial &mono) const {
    return this->const_term.eq(mono.term());
}

Var PolyMgr::getVar(std::string name, int bits) {
    if (this->var_pool.find(name) == this->var_pool.end()) {
        Var var(name, bits);
        this->var_pool[name] = var;
        return var;
    } else {
        Var var = this->var_pool[name];
        if (var.bits() != bits) {
            printf("[PolyMgr] Inconsistent bit width for variable \"%s\"!\n", name.c_str());
        }
        return var;
    }
}

Term PolyMgr::getTerm(std::set<std::string> &vars_str, std::vector<Var> &vars, std::vector<int> &expos) {
    if (this->term_pool.find(vars_str) == this->term_pool.end()) {
        Term term(vars, expos);
        this->term_pool[vars_str] = term;
        return term;
    } else {
        return this->term_pool[vars_str];
    }
}

Polynomial PolyMgr::getPoly(Var &var) {
    std::set<std::string> var_str = {std::string(var.name()) + " 1"};
    std::vector<Var> vars = {var};
    std::vector<int> expo = {1};
    Term term = this->getTerm(var_str, vars, expo);
    mpz_set_si(this->temp1, 1);
    std::vector<Monomial> mono = {Monomial(term, this->temp1)};
    return this->getPoly(mono);
}

Polynomial PolyMgr::getPoly(Monomial &mono) {
    std::vector<Monomial> mono_vec = {mono};
    return this->getPoly(mono_vec);
}

Polynomial PolyMgr::getPoly(std::vector<Monomial> &monos) {
    if (monos.size() == 0) return null_poly;

    Polynomial polynomial(monos);
    polynomial.sort();
    return polynomial;
}

Polynomial PolyMgr::getConst(long coeff) {
    if (coeff == 0) return this->null_poly;
    mpz_set_si(this->temp1, coeff);
    std::vector<Monomial> monos = {Monomial(const_term, this->temp1)};

    return this->getPoly(monos);
}

Polynomial PolyMgr::getConst(mpz_t coeff) {
    if (mpz_cmp_si(coeff, 0) == 0) return this->null_poly;
    std::vector<Monomial> monos = {Monomial(const_term, coeff)};

    return this->getPoly(monos);
}

Monomial PolyMgr::mono_true_div(Monomial &dividend, Monomial &divisor) {
    if (this->is_const(divisor)) {
        mpz_fdiv_q(this->temp1, MONO_COEFF(dividend), MONO_COEFF(divisor));

        if (mpz_cmp_si(this->temp1, 0) != 0) {
            Monomial quo(dividend.term(), this->temp1);

            return quo; 
        }
        return Monomial();
    }
    Term divisor_term  = divisor.term();
	Term dividend_term = dividend.term();
    std::vector<int> term_expo = TERM_EXPO(dividend_term);
	for (int i = 0; i < divisor_term.nvars(); ++i) {
		TermElem elem_divisor = divisor_term[i];
		// check vars and expo
		bool exist = false;
		for (int j = 0; j < dividend.nvars(); ++j) {
			TermElem elem_dividend = dividend_term[j];
			if (elem_dividend.var.eq(elem_divisor.var)) {
				if (elem_dividend.expo >= elem_divisor.expo) {
                    term_expo[j] = elem_dividend.expo - elem_divisor.expo;
					exist = true;
					break;
				}
			}
		}
		if (!exist) return Monomial();
	}
    // can be divided
    mpz_fdiv_q(this->temp1, MONO_COEFF(dividend), MONO_COEFF(divisor));

    std::set<std::string> quo_vars_str;
    std::vector<Var> quo_vars;
    std::vector<int> quo_expo;
    for (int i = 0; i < dividend.nvars(); ++i) {
        if (term_expo[i]) {
            quo_vars.push_back(dividend_term[i].var);
            quo_expo.push_back(term_expo[i]);
            quo_vars_str.insert(std::string(dividend_term[i].var.name()) + " " + std::to_string(term_expo[i]));
        }
    }

    Term quo_term = this->const_term;
    if (quo_vars.size() > 0) {
        quo_term = this->getTerm(quo_vars_str, quo_vars, quo_expo);
    }

    Monomial quo(quo_term, this->temp1);

    return quo;
}

Monomial PolyMgr::mono_mul(Monomial &a, Monomial &b) {
    Term mul_term;
    if (this->is_const(a)) {
        mul_term = b.term();
    } else if (this->is_const(b)) {
        mul_term = a.term();
    } else {
        std::set<Var> a_vars = a.vars();
        std::set<Var> b_vars = b.vars();
        std::set<Var> mul_vars_set;
        mul_vars_set.insert(a_vars.begin(), a_vars.end());
        mul_vars_set.insert(b_vars.begin(), b_vars.end());

        std::set<std::string> mul_vars_str;
        std::vector<Var> mul_vars(mul_vars_set.begin(), mul_vars_set.end());
        std::vector<int> mul_expo(mul_vars.size());
        for (int i = 0; i < mul_vars.size(); ++i) {
            mul_expo[i] = a.var_expo(mul_vars[i]) + b.var_expo(mul_vars[i]);
            mul_vars_str.insert(std::string(mul_vars[i].name()) + " " + std::to_string(mul_expo[i]));
        }
        mul_term = this->getTerm(mul_vars_str, mul_vars, mul_expo);
    }
    mpz_mul(this->temp1, MONO_COEFF(a), MONO_COEFF(b));

    Monomial mul_mono(mul_term, this->temp1);
    return mul_mono;
}

Polynomial PolyMgr::parse(std::string poly) {
    std::stringstream ss(poly);
    char checkpoint;
    std::string name, coeff;
    int bits, expo;
    std::vector<Monomial> monos;

    while (ss.tellg() != ss.str().length()) {
        ss >> checkpoint;
        if (checkpoint != '(') {
            assert(false);
        }
        ss >> coeff;
        if (coeff.find(')') != std::string::npos) { // CONST
            coeff = coeff.substr(0, coeff.find(')'));
            mpz_init_set_str(this->temp1, coeff.c_str(), 10);
            Monomial mono(const_term, this->temp1);
            monos.push_back(mono);
            if (ss.tellg() == -1) break;
        } else {
            std::set<std::string> vars_str;
            std::vector<Var> vars;
            std::vector<int> expos;
            do {
                ss >> bits >> name >> expo;

                Var var = this->getVar(name, bits);
                vars_str.insert(name + " " + std::to_string(expo));
                vars.push_back(var);
                expos.push_back(expo);

                ss >> checkpoint;
                if (checkpoint != ')') {
                    ss.putback(checkpoint);
                } else {
                    break;
                }
            } while (1);
            mpz_init_set_str(this->temp1, coeff.c_str(), 10);
            Term term = this->getTerm(vars_str, vars, expos);
            Monomial mono(term, this->temp1);
            monos.push_back(mono);
        }
    }

    return this->getPoly(monos);
}

Polynomial PolyMgr::poly_module(Polynomial &a, mpz_t coeff) {
    Polynomial poly;
    for (int i = 0; i < a.length(); ++i) {
        Monomial mono = a[i];
        mpz_mod(this->temp1, MONO_COEFF(mono), coeff);
        if (mpz_cmp_si(this->temp1, 0) == 0) continue;
        Term term = mono.term();
        Monomial mono_updated(term, this->temp1);
        Polynomial poly_sub = this->getPoly({mono_updated});
        poly = this->poly_add(poly, poly_sub);
    }
    return poly;
}

Polynomial PolyMgr::poly_neg(Polynomial &a) {
    if (a.is_zero()) return Polynomial();

    std::vector<Monomial> monos;
    for (int i = 0; i < a.length(); ++i) {
        Monomial mono = a[i];
        Term term = mono.term();
        mpz_neg(this->temp1, MONO_COEFF(mono));
        Monomial mono_new(term, this->temp1);
        monos.push_back(mono_new);
    }

    return this->getPoly(monos);
}

Polynomial PolyMgr::poly_add(Polynomial &a, Polynomial &b) {
    if (a.is_zero()) return b;
    if (b.is_zero()) return a;

    std::vector<Monomial> add_monos;
    std::vector<int> a_processed(a.length(), 0);
    std::vector<int> b_processed(b.length(), 0);

    for (int i = 0; i < a.length(); ++i) {
        Monomial mono_a = a[i];
        for (int j = 0; j < b.length(); ++j) {
            if (b_processed[j]) continue;
            Monomial mono_b = b[j];

            if (mono_a.term_eq(mono_b)) {
                a_processed[i] = 1;
                b_processed[j] = 1;

                mpz_add(this->temp1, MONO_COEFF(mono_a), MONO_COEFF(mono_b));
                if (mpz_cmp_si(this->temp1, 0) != 0) {
                    Term term = mono_a.term();
                    Monomial mono(term, this->temp1);
                    add_monos.push_back(mono);
                }
            }
        }
    }
    for (int i = 0; i < a.length(); ++i) {
        if (a_processed[i] == 0) {
            Monomial mono_a = a[i];
            add_monos.push_back(mono_a);
        }
    }
    for (int i = 0; i < b.length(); ++i) {
        if (b_processed[i] == 0) {
            Monomial mono_b = b[i];
            add_monos.push_back(mono_b);
        }
    }

	return this->getPoly(add_monos);
}

Polynomial PolyMgr::poly_sub(Polynomial &a, Polynomial &b) {
    Polynomial c = this->poly_neg(b);
    return this->poly_add(a, c);
}

Polynomial PolyMgr::poly_mul(Polynomial &a, Polynomial &b) {
    if (a.empty() || b.empty()) return Polynomial();
    Polynomial mul_poly;
    for (int i = 0; i < a.length(); ++i) {
        Monomial mono_a = a[i];
        for (int j = 0; j < b.length(); ++j) {
            Monomial mono_b = b[j];

            std::vector<Monomial> mono_mul = {this->mono_mul(mono_a, mono_b)};
            Polynomial poly_mono = this->getPoly(mono_mul);
            mul_poly = this->poly_add(mul_poly, poly_mono);
        }
    }
    return mul_poly;
}

// void PolyMgr::poly_div(Polynomial &quo, Polynomial &rem, Polynomial &dividend, Polynomial &divisor) {
//     std::vector<std::vector<Monomial> > quos;
//     for (int i = 0; i < divisor.length(); ++i) {
//         Monomial divisor_mono = divisor[i];

//         std::vector<Monomial> quos_;
//         for (int j = 0; j < dividend.length(); ++j) {
//             Monomial dividend_mono = dividend[j];

//             Monomial quo = this->mono_true_div(dividend_mono, divisor_mono);
//             if (!quo.empty()) {
//                 quos_.push_back(quo);
//             }
//         }
//         quos.push_back(quos_);
//     }
//     std::vector<Monomial> quo_mono = quos[0];
//     std::vector<Monomial> quo_temp;
//     for (int i = 1; i < quos.size(); ++i) {
//         for (auto mono : quos[i]) {
//             for (auto mono_quo : quo_mono) {
//                 if (mono.eq(mono_quo)) {
//                     quo_temp.push_back(mono_quo);
//                 }
//             }
//         }
//         quo_mono = quo_temp;
//     }
//     quo = this->getPoly(quo_mono);
//     Polynomial p = this->poly_mul(quo, divisor);
//     // p.print(stdout); printf("\n");
//     rem = this->poly_sub(dividend, p);
// }

void PolyMgr::poly_div(Polynomial &quo, Polynomial &rem, Polynomial dividend, Polynomial &divisor) {
    int leading_expo = divisor[0].expo();
    for (int j = 0; j < divisor.length() && divisor[j].expo() >= leading_expo; ++j) {
        Monomial mono_divisor = divisor[j];
        for (int i = 0; i < dividend.length(); ++i) {
            Monomial mono_dividend = dividend[i];
            Monomial mono_div = this->mono_true_div(mono_dividend, mono_divisor);
            if (!mono_div.empty()) {
                Polynomial quo_poly = this->getPoly(mono_div);
                quo = this->poly_add(quo, quo_poly);
                quo_poly = this->poly_mul(quo_poly, divisor);
                dividend = this->poly_sub(dividend, quo_poly);
                break;
            }
        }
    }
    rem = dividend;
}

Polynomial PolyMgr::getVanishPoly(Var &var, mpz_class k) {
    Polynomial vanish_var_poly = this->getPoly(var);
    Polynomial vanish_poly = this->getConst(1);
    int degree = mpz_get_si(k.get_mpz_t());
    for (int i = 0; i < degree; ++i) {
        Polynomial vanish_sub_offset = this->getConst(i);
        Polynomial vanish_sub_poly = this->poly_sub(vanish_var_poly, vanish_sub_offset);
        // printf("Sub: "); vanish_sub_poly.print(stdout); printf("\n");
        vanish_poly = this->poly_mul(vanish_poly, vanish_sub_poly);
    }

    return vanish_poly;
}

Polynomial PolyMgr::getVanishHatPoly(std::vector<Var> &vars, std::vector<mpz_class> k) {
    assert(vars.size() == k.size());
    Polynomial vanish_poly = this->getConst(1);
    for (int i = 0; i < vars.size(); ++i) {
        Polynomial vansih_sub_poly = this->getVanishPoly(vars[i], k[i]);
        vanish_poly = this->poly_mul(vanish_poly, vansih_sub_poly);
    }
    return vanish_poly;
}

bool PolyMgr::poly_eq(Polynomial &a, Polynomial &b, int bits) {
    Polynomial poly = this->poly_sub(a, b);
    if (poly.empty()) return true;

    // Collect all input variables
    std::set<Var> vars_a = a.vars();
    std::set<Var> vars_b = b.vars();
    std::set<Var> vars_set;
    vars_set.insert(vars_a.begin(), vars_a.end());
    vars_set.insert(vars_b.begin(), vars_b.end());
    vars_set.erase(this->const_var);
    std::vector<Var> vars(vars_set.begin(), vars_set.end());

#ifdef DEBUG
    printf("Input variables: ");
    for (auto var : vars) {
        var.print(stdout); printf("(%d), ", var.bits());
    }
    printf("\n");
#endif

    // Calcualte Range
    mpz_t range;
    mpz_init(range);
    mpz_ui_pow_ui(range, 2, bits);

#ifdef DEBUG
    printf("The range of the output is [0, ");
    mpz_out_str(stdout, 10, range);
    printf(")\n");
#endif

    poly = this->poly_module(poly, range);

#ifdef DEBUG
    printf("Checking the polynomial: ");
    poly.print(stdout); printf("\n");
#endif

    if (poly.empty()) return true;

    // Calculate SF
    unsigned int sf;
    for (sf = 1; ; ++sf) {
        mpz_fac_ui(temp1, sf);
        if (mpz_divisible_p(temp1, range)) {
            break;
        }
    }

#ifdef DEBUG
    printf("SF: %d\n", sf);
#endif

    // Calculate u, k
    std::vector<mpz_class> u(vars.size());
    for (size_t i = 0; i < vars.size(); ++i) {
        mpz_class range;
        mpz_ui_pow_ui(range.get_mpz_t(), 2, vars[i].bits());
        u[i] = mpz_cmp_ui(range.get_mpz_t(), sf) >= 0 ? sf : range;
    }

#ifdef DEBUG
    printf("u: ");
    for (size_t i = 0; i < vars.size(); ++i) {
        mpz_out_str(stdout, 10, u[i].get_mpz_t()); printf(", ");
    }
    printf("\n");
#endif

    std::vector<mpz_class> k(vars.size());
    for (size_t i = 0; i < vars.size(); ++i) {
        k[i] = poly.var_max_expo(vars[i]);
    }

#ifdef DEBUG
    printf("k: ");
    for (size_t i = 0; i < vars.size(); ++i) {
        mpz_out_str(stdout, 10, k[i].get_mpz_t()); printf(", ");
    }
    printf("\n");
#endif

    // Check if Y_u divide poly
    for (int i = 0; i < vars.size(); ++i) {
        if (mpz_cmp(k[i].get_mpz_t(), u[i].get_mpz_t()) >= 0) {
            Polynomial vanish_poly = this->getVanishPoly(vars[i], k[i]);
#ifdef DEBUG
            print_poly(poly);
            print_poly(vanish_poly);
#endif
            Polynomial quo, rem;
            this->poly_div(quo, rem, poly, vanish_poly);

#ifdef DEBUG
            print_poly(quo);
            print_poly(rem);
#endif
            if (rem.empty()) {
                return true;
            } else {
                poly = rem;
                // break; // Need break here?
            }
        }
    }

    // // Iterate all possible degree
    mpz_t j;
    mpz_init(j);
    mpz_set_ui(j, 1);
    for (int i = 0; i < u.size(); ++i) {
        mpz_mul(j, j, u[i].get_mpz_t());
    }
    int haha = 0;
    // print_mpz(j);
    for ( ; mpz_cmp_ui(j, 0) >= 0; mpz_sub_ui(j, j, 1)) {
        if (poly.empty()) return true;
        // update k
        Monomial max_expo = poly[0];
        for (size_t i = 0; i < vars.size(); ++i) {
            k[i] = max_expo.var_expo(vars[i]);
        }

        Polynomial vanish_poly = this->getVanishHatPoly(vars, k);
#ifdef DEBUG
        print_poly(poly);
        print_poly(vanish_poly);
#endif
        Polynomial quo, rem;
        this->poly_div(quo, rem, poly, vanish_poly);
#ifdef DEBUG
        print_poly(quo);
        print_poly(rem);
#endif
        
        // calculate k factorial
        mpz_t k_hat;
        mpz_init(k_hat);
        mpz_set_ui(k_hat, 1);
        for (int i = 0; i < vars.size(); ++i) {
            mpz_fac_ui(this->temp1, mpz_get_ui(k[i].get_mpz_t()));
            mpz_mul(k_hat, k_hat, this->temp1);
        }
        // print_mpz(k_hat);
        mpz_gcd(this->temp1, range, k_hat);
        mpz_clear(k_hat);

        if (quo.empty()) return false;
        // calculate b
        mpz_t b;
        mpz_init(b);
        mpz_fdiv_q(b, range, this->temp1);
        // print_mpz(b);

        Polynomial b_poly = this->getConst(b);
        Polynomial b_quo, b_rem;
        this->poly_div(b_quo, b_rem, quo, b_poly);
        // print_poly(b_rem);
        if (b_rem.empty()) {
            if (rem.empty()) {
                return true;
            } else {
                poly = rem;
                poly = this->poly_module(poly, range);
            }
        } else {
            mpz_clear(j);
            mpz_clear(range);
            return false;
        }
    }


    mpz_clear(j);
    mpz_clear(range);
    return false;
}

} // namespace sword