#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "Monomial.h"

#include <cassert>
#include <vector>

namespace sword {

struct PolynomialBase {
	PolynomialBase() { mpz_init(ring_z); }
	~PolynomialBase() { mpz_clear(ring_z); }
	std::vector<Monomial> monomials;
	Monomial null_monomial;
	mpz_t ring_z;
};

class SWORD_EXPORT Polynomial {
public:
	// empty
	Polynomial();
	// instantiate
	Polynomial(std::vector<Monomial> monomials);
	// copy
	Polynomial(const Polynomial &other);
	// release
	~Polynomial();
	// assign
	Polynomial& operator=(const Polynomial& other);
	// allocate
	void create();
	// refcount++
	void addref();
	// refcount--
	void release();
	// empty
	bool empty() const;
	// is zero
	bool is_zero() const;

	Monomial& operator[](size_t i);
	const Monomial& operator[](size_t i) const;

	int  length() const;
	void print(FILE *f, bool detail = false) const;
	std::string str() const;
	void sort() const;
	int  highest_order() const;
	int  highest_bits() const;
	void ring_z() const;

	std::set<Var> vars() const;
	int var_max_expo(Var &var) const;

	// pointer to data
	PolynomialBase *data;
	// reference counter
	int *refcount;
};

SWORD_FORCEINLINE Polynomial::Polynomial() : data(0), refcount(0) {}

SWORD_FORCEINLINE Polynomial::Polynomial(std::vector<Monomial> monomials) : data(0), refcount(0) {
	create();
	((PolynomialBase*)data)->monomials = monomials;
	this->ring_z();
}

SWORD_FORCEINLINE Polynomial::Polynomial(const Polynomial &other) : data(other.data), refcount(other.refcount) { addref(); }

SWORD_FORCEINLINE Polynomial::~Polynomial() { release(); }

SWORD_FORCEINLINE Polynomial& Polynomial::operator=(const Polynomial &other) {
	if (this == &other) return *this;
	if (other.refcount) XADD(other.refcount, 1);
	release();
	data = other.data;
	refcount = other.refcount;
	return *this;
}

SWORD_FORCEINLINE void Polynomial::addref() { if (refcount) XADD(refcount, 1); }

SWORD_FORCEINLINE void Polynomial::release() {
	if (refcount && XADD(refcount, -1) == 1) {
		delete data;
		delete refcount;
		// printf("Polynomial Free!\n"); 
	}
	data = 0;
	refcount = 0;
}

SWORD_FORCEINLINE bool Polynomial::empty() const {
	return this->data == 0;
}

SWORD_FORCEINLINE Monomial& Polynomial::operator[](size_t i) {
	if (i >= this->length() || this->empty()) return ((PolynomialBase*)data)->null_monomial;
	return ((PolynomialBase*)data)->monomials[i];
}

SWORD_FORCEINLINE const Monomial& Polynomial::operator[](size_t i) const {
	if (i >= this->length() || this->empty()) return ((PolynomialBase*)data)->null_monomial;
	return ((PolynomialBase*)data)->monomials[i];
}

} // namespace sword

#endif // POLYNOMIAL_H
