#ifndef MONOMIAL_H
#define MONOMIAL_H

#include "SmartObj.h"
#include "Term.h"
#include <gmp.h>

namespace sword {

#define MONO_COEFF(mono) ((MonomialBase*)mono.data)->coeff

struct MonomialBase {
	~MonomialBase() { mpz_clear(coeff); }
	Term  term;
	mpz_t coeff;
};

class SWORD_EXPORT Monomial : public SmartObj<MonomialBase> {
public:
	// empty
	Monomial();
	// instantiate
	Monomial(Term term, mpz_t coeff);
	// allocate
	void create();
	// print
	void print(FILE *f, bool detail = false) const;
	std::string str() const;

	bool operator<(const Monomial &other) const;

	// monomial
	bool eq(Monomial &other) const;
	int expo() const; // total expo
	int max_bits() const;

	// term
	Term term() const;
	bool term_eq(Monomial other) const;
	bool term_contain(Monomial &mono) const;

	// vars
	int nvars() const;
	std::set<Var> vars() const;
	int  var_expo(Var &var) const; // var expo
	bool var_contain(Var &var) const;
};

SWORD_FORCEINLINE Monomial::Monomial() : SmartObj() {}

SWORD_FORCEINLINE Monomial::Monomial(Term term, mpz_t coeff) : SmartObj() {
	create();
	((MonomialBase*)data)->term = term;
	mpz_init_set(((MonomialBase*)data)->coeff, coeff);
}

SWORD_FORCEINLINE bool Monomial::operator<(const Monomial &other) const {
	return this->data < other.data;
}

} // namespace sword

#endif // MONOMIAL_H
