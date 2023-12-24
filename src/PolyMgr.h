#ifndef POLYMGR_H
#define POLYMGR_H

#include "Var.h"
#include "Term.h"
#include "Monomial.h"
#include "Polynomial.h"

#include <gmp.h>
#include <gmpxx.h>
#include <string.h>
#include <set>
#include <map>
#include <unordered_map>

// #define DEBUG 1

namespace sword {

#define print_poly(poly) \
do { \
    printf("%s: ", #poly); poly.print(stdout); printf("\n"); \
} while (0)

#define print_mpz(mpz) \
do { \
    printf("%s: ", #mpz); mpz_out_str(stdout, 10, mpz); printf("\n"); \
} while (0)

class SWORD_EXPORT PolyMgr {
public:
    PolyMgr();
    ~PolyMgr();

    // Monomial
    Monomial mono_true_div(Monomial &dividend, Monomial &divisor);
    Monomial mono_mul(Monomial &a, Monomial &b);

    // Polynomial
    Polynomial parse(std::string poly);
    Polynomial poly_module(Polynomial &a, mpz_t coeff);
    Polynomial poly_neg(Polynomial &a);
    Polynomial poly_add(Polynomial &a, Polynomial &b);
    Polynomial poly_sub(Polynomial &a, Polynomial &b);
    Polynomial poly_mul(Polynomial &a, Polynomial &b);
    void poly_div(Polynomial &quo, Polynomial &rem, Polynomial dividend, Polynomial &divisor);
    Polynomial getConst(long coeff);
    Polynomial getConst(mpz_t coeff);

    bool poly_eq(Polynomial &a, Polynomial &b, int bits);

private:
    Var getVar(std::string name, int bits);
    Term getTerm(std::set<std::string> &vars_str, std::vector<Var> &vars, std::vector<int> &expos);
    Polynomial getPoly(Var &var);
    Polynomial getPoly(Monomial &mono);
    Polynomial getPoly(std::vector<Monomial> &monos);
    
    Polynomial getVanishPoly(Var &var, mpz_class k);
    Polynomial getVanishHatPoly(std::vector<Var> &vars, std::vector<mpz_class> k);

    bool is_const(Term &term) const;
    bool is_const(Monomial &mono) const;

private:
    Var const_var;
    Term const_term;
    Polynomial null_poly;
    std::unordered_map<std::string, Var> var_pool;
    std::map<std::set<std::string>, Term> term_pool;

    mpz_t temp1;
};

SWORD_FORCEINLINE PolyMgr::PolyMgr() {
    mpz_init(temp1);
    const_var = Var("CONST", 0);
    const_term = Term({const_var}, {0});
}

SWORD_FORCEINLINE PolyMgr::~PolyMgr() {
    mpz_clear(temp1);
}

} // namespace sword

#endif // POLYMGR_H