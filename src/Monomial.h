#ifndef MONOMIAL_H
#define MONOMIAL_H

#include "Term.h"
#include <gmp.h>

namespace sword {

#define MONO_COEFF(mono) ((MonomialBase*)mono.data)->coeff

struct MonomialBase {
	~MonomialBase() { mpz_clear(coeff); }
	Term  term;
	mpz_t coeff;
};

class SWORD_EXPORT Monomial {
public:
	// empty
	Monomial();
	// instantiate
	Monomial(Term term, mpz_t coeff);
	// copy
	Monomial(const Monomial &other);
	// release
	~Monomial();
	// assign
	Monomial& operator=(const Monomial& other);
	// allocate
	void create();
	// refcount++
	void addref();
	// refcount--
	void release();
	// empty
	bool empty() const;
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

	// pointer to data
	MonomialBase *data;
	// reference counter
	int *refcount;
};

SWORD_FORCEINLINE Monomial::Monomial() : data(0), refcount(0) {}

SWORD_FORCEINLINE Monomial::Monomial(Term term, mpz_t coeff) : data(0), refcount(0) {
	create();
	((MonomialBase*)data)->term = term;
	mpz_init_set(((MonomialBase*)data)->coeff, coeff);
}

SWORD_FORCEINLINE Monomial::Monomial(const Monomial &other) : data(other.data), refcount(other.refcount) { addref(); }

SWORD_FORCEINLINE Monomial::~Monomial() { release(); }

SWORD_FORCEINLINE Monomial& Monomial::operator=(const Monomial &other) {
	if (this == &other) return *this;
	if (other.refcount) XADD(other.refcount, 1);
	release();
	data = other.data;
	refcount = other.refcount;
	return *this;
}

SWORD_FORCEINLINE void Monomial::addref() { if (refcount) XADD(refcount, 1); }

SWORD_FORCEINLINE void Monomial::release() {
	if (refcount && XADD(refcount, -1) == 1) {
		delete data;
		delete refcount;
		// printf("Monomial Free!\n");
	}
	data = 0;
	refcount = 0;
}

SWORD_FORCEINLINE bool Monomial::empty() const {
	return this->data == 0;
}

SWORD_FORCEINLINE bool Monomial::operator<(const Monomial &other) const {
	return this->data < other.data;
}

} // namespace sword

#endif // MONOMIAL_H
