#ifndef TERM_H
#define TERM_H

#include "SmartObj.h"
#include "Var.h"

#include <set>
#include <cassert>
#include <vector>

namespace sword {

#define TERM_EXPO(term) ((TermBase*)term.data)->expos

struct TermBase {
	std::vector<Var> vars;
	std::vector<int> expos;
};

struct TermElem {
	Var var;
	int expo;
};

class SWORD_EXPORT Term : public SmartObj<TermBase> {
public:
	// empty
	Term();
	// instantiate
	Term(std::vector<Var> vars, std::vector<int> expos);
	// allocate
	void create();
	// print
	void print(FILE *f, bool detail = false) const;
	std::string str() const;

	const TermElem operator[](size_t i) const;
	bool operator<(const Term &other) const;
	bool operator==(const Term &other) const;
	bool eq(Term other) const;

	int  nvars() const;
	int  expo() const;

	// vars
	std::set<Var> vars() const;
	int  var_expo(Var &var) const;
	bool var_contain(Var &var) const;
};

SWORD_FORCEINLINE Term::Term() : SmartObj() {}

SWORD_FORCEINLINE Term::Term(std::vector<Var> vars, std::vector<int> expos) : SmartObj() {
	create();
	assert(vars.size() == expos.size());
	((TermBase*)data)->vars = vars;
	((TermBase*)data)->expos = expos;
}

SWORD_FORCEINLINE bool Term::operator<(const Term& other) const {
	return false;
}

SWORD_FORCEINLINE bool Term::operator==(const Term& other) const {
	return this->eq(other);
}

SWORD_FORCEINLINE const TermElem Term::operator[](size_t i) const {
	return {((TermBase*)data)->vars[i], ((TermBase*)data)->expos[i]};
}

} // namespace sword

#endif // TERM_H
