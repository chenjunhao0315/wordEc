#include "Monomial.h"

#include <string>

namespace sword {

void Monomial::create() {
	data = new MonomialBase;
	refcount = new int;
	*refcount = 1;
}

void Monomial::print(FILE *f, bool detail) const {
	mpz_out_str(f, 10,((MonomialBase*)data)->coeff);
	fprintf(f, "*");
	((MonomialBase*)data)->term.print(f, detail);
}

bool Monomial::eq(Monomial &other) const {
	return mpz_cmp(((MonomialBase*)data)->coeff, MONO_COEFF(other)) == 0 && this->term_eq(other);
}

int Monomial::expo() const {
	return this->term().expo();
}

int Monomial::var_expo(Var &var) const {
	return this->term().var_expo(var);
}

int Monomial::max_bits() const {
	int bits = 0;
	for (size_t i = 0; i < ((MonomialBase*)data)->term.nvars(); ++i) {
		bits = std::max(((MonomialBase*)data)->term[i].var.bits(), bits);
	}
	return bits;
}

int Monomial::nvars() const {
	return this->term().nvars();
}

std::set<Var> Monomial::vars() const {
	return this->term().vars();
}

Term Monomial::term() const {
	return ((MonomialBase*)data)->term;
}

bool Monomial::term_eq(Monomial other) const {
	return this->term().eq(other.term());
}

bool Monomial::term_contain(Monomial &mono) const {
	Term mono_term = mono.term();
	Term self_term = this->term();
	for (int i = 0; i < mono.nvars(); ++i) {
		TermElem elem_mono = mono_term[i];
		// check vars and expo
		bool exist = false;
		for (int j = 0; j < this->nvars(); ++j) {
			TermElem elem_self = self_term[j];
			if (elem_self.var.eq(elem_mono.var)) {
				if (elem_self.expo >= elem_mono.expo) {
					exist = true;
					break;
				}
			}
		}
		if (!exist) return false;
	}
	return true;
}

bool Monomial::var_contain(Var &var) const {
	return this->term().var_contain(var);
}

} // namespace sword

