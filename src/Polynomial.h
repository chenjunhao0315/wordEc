#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "SmartObj.h"
#include "Monomial.h"

#include <cassert>
#include <vector>

namespace sword {

struct PolynomialBase {
	std::vector<Monomial> monomials;
	Monomial null_monomial;
};

class SWORD_EXPORT Polynomial : public SmartObj<PolynomialBase> {
public:
	// emtpy
	Polynomial();
	// instantiate
	Polynomial(std::vector<Monomial> monomials);
	// allocate
	void create();
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

	std::set<Var> vars() const;
	int var_max_expo(Var &var) const;
};

SWORD_FORCEINLINE Polynomial::Polynomial() : SmartObj() {}

SWORD_FORCEINLINE Polynomial::Polynomial(std::vector<Monomial> monomials) : SmartObj() {
	create();
	((PolynomialBase*)data)->monomials = monomials;
	this->ring_z();
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
