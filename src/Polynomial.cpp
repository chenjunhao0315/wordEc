#include "Polynomial.h"

#include <algorithm>

namespace sword {

void Polynomial::create() {
	data = new PolynomialBase;
	refcount = new int;
	*refcount = 1;
}

int Polynomial::length() const {
	return ((PolynomialBase*)data)->monomials.size();
}

void Polynomial::print(FILE *f, bool detail) const {
	if (data) {
		for (size_t i = 0; i < length(); ++i) {
			auto &mono = operator[](i);
			fprintf(f, " ");
			if (mpz_sgn(((MonomialBase*)mono.data)->coeff) >= 0)
				fprintf(f, "+");
			mono.print(f, detail);
		}
	}
}

void Polynomial::sort() const {
	auto& monos = ((PolynomialBase*)data)->monomials;

	std::sort(monos.begin(), monos.end(), [](const Monomial &a, const Monomial &b) {
		if (a.expo() == b.expo())
			return a.term().data < b.term().data;
		return a.expo() > b.expo();
	});
}

int Polynomial::highest_order() const {
	this->sort();
	return ((PolynomialBase*)data)->monomials[0].expo();
}

int Polynomial::highest_bits() const {
	int bits = 0;
	for (size_t i = 0; i < this->length(); ++i) {
		bits = std::max(operator[](i).max_bits(), bits);
	}
	return bits;
}

void Polynomial::ring_z() const {
	int bits = highest_bits();
	mpz_t expo;
	mpz_init(expo);
	mpz_set_ui(expo, 2);
	mpz_pow_ui(((PolynomialBase*)data)->ring_z, expo, bits);
	mpz_clear(expo);
}

std::set<Var> Polynomial::vars() const {
	std::set<Var> vars;

	for (int i = 0; i < this->length(); ++i) {
		std::set<Var> vars_mono = this->operator[](i).vars();
		vars.insert(vars_mono.begin(), vars_mono.end());
	}

	return vars;
}

int Polynomial::var_max_expo(Var &var) const {
	int expo = 0;
	for (int i = 0; i < this->length(); ++i) {
		Monomial mono = this->operator[](i);
		expo = std::max(expo, mono.var_expo(var));
	}

	return expo;
}

bool Polynomial::is_zero() const {
	return this->empty() || (this->length() == 1 && mpz_cmp_si(MONO_COEFF(this->operator[](0)), 0) == 0);
}

} // namespace sword
