#include "Term.h"

namespace sword {

void Term::create() {
	data = new TermBase;
	refcount = new int;
	*refcount = 1;
}

int Term::nvars() const {
	return ((TermBase*)data)->vars.size();
}

std::set<Var> Term::vars() const {
	return std::set<Var>(((TermBase*)data)->vars.begin(), ((TermBase*)data)->vars.end());
}

int Term::expo() const {
	int expo = 0;
	for (int i = 0; i < this->nvars(); ++i) {
		expo += this->operator[](i).expo;
	}
	return expo;
}

int Term::var_expo(Var &var) const {
	for (int i = 0; i < this->nvars(); ++i) {
		TermElem elem = this->operator[](i);
		if (var.eq(elem.var)) {
			return elem.expo;
		}
	}
	return 0;
}

void Term::print(FILE *f, bool detail) const {
	if (data) {
		for (size_t i = 0; i < nvars(); ++i) {
			auto &term = operator[](i);
			term.var.print(f, detail);
			fprintf(f, "^%d", term.expo);
			if (i != nvars()-1)
				fprintf(f, "*");
		}
	} else {
		fprintf(f, "Empty Term!");
	}
}

std::string Term::str(void) const {
	std::string str;
	if (data) {
		for (size_t i = 0; i < nvars(); ++i) {
			auto &term = operator[](i);
			str += term.var.str();
			str += "^" + std::to_string(term.expo);
			if (i != nvars()-1)
				str += "*";
		}
	}
	return str;
}

bool Term::eq(Term other) const {
	return this->data == other.data;
}

bool Term::var_contain(Var &var) const {
	// TODO: better search
	for (int i = 0; i < this->nvars(); ++i) {
		TermElem elem = this->operator[](i);
		if (var.eq(elem.var)) {
			return true;
		}
	}
	return false;
}

} // namespace sword
