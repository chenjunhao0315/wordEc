#ifndef TERM_H
#define TERM_H

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

class SWORD_EXPORT Term {
public:
	// empty
	Term();
	// instantiate
	Term(std::vector<Var> vars, std::vector<int> expos);
	// copy
	Term(const Term &other);
	// release
	~Term();
	// assign
	Term& operator=(const Term& other);
	// deep copy
	Term clone() const;
	// deep copy from other
	void clone_from(const Term &other);
	// allocate
	void create();
	// refcount++
	void addref();
	// refcount--
	void release();
	// print
	void print(FILE *f, bool detail = false) const;

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

	// pointer to data
	TermBase *data;
	// reference counter
	int *refcount;
};

SWORD_FORCEINLINE Term::Term() : data(0), refcount(0) {}

SWORD_FORCEINLINE Term::Term(std::vector<Var> vars, std::vector<int> expos) : data(0), refcount(0) {
	create();
	assert(vars.size() == expos.size());
	((TermBase*)data)->vars = vars;
	((TermBase*)data)->expos = expos;
}

SWORD_FORCEINLINE Term::Term(const Term &other) : data(other.data), refcount(other.refcount) { addref(); }

SWORD_FORCEINLINE Term::~Term() { release(); }

SWORD_FORCEINLINE Term& Term::operator=(const Term &other) {
	if (this == &other) return *this;
	if (other.refcount) XADD(other.refcount, 1);
	release();
	data = other.data;
	refcount = other.refcount;
	return *this;
}

SWORD_FORCEINLINE void Term::addref() { if (refcount) XADD(refcount, 1); }

SWORD_FORCEINLINE void Term::release() {
	if (refcount && XADD(refcount, -1) == 1) {
		delete data;
		delete refcount;
		// printf("Term Free!\n"); 
	}
	data = 0;
	refcount = 0;
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
