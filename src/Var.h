#ifndef VAR_H
#define VAR_H

#include "platform.h"
#include "allocator.h"
#include <string>

namespace sword {

struct VarBase {
    std::string name;
    int bits;
    int hash;
};

class SWORD_EXPORT Var {
public:
	// empty
	Var();
    // instantiate
    Var(std::string name, int bits);
	// copy
	Var(const Var &other);
	// release
	~Var();
	// assign
	Var& operator=(const Var& other);
	// allocate
	void create();
	// refcount++
	void addref();
	// refcount--
	void release();
    // print
    void print(FILE *f, bool detail = false) const;
    // get name
    const char *name() const;
    // get bits
    int bits() const;
    // get hash
    int hash() const;

	bool operator<(const Var &other) const;
	bool eq(Var var) const;

	// pointer to data
	VarBase *data;
	// reference counter
	int *refcount;
};

SWORD_FORCEINLINE Var::Var() : data(0), refcount(0) {}

SWORD_FORCEINLINE Var::Var(std::string name, int bits) : data(0), refcount(0) {
	create();
	((VarBase*)data)->name = name;
    ((VarBase*)data)->bits = bits;
	((VarBase*)data)->hash = std::hash<std::string>()(name);
}

SWORD_FORCEINLINE Var::Var(const Var &other) : data(other.data), refcount(other.refcount) { addref(); }

SWORD_FORCEINLINE Var::~Var() { release(); }

SWORD_FORCEINLINE Var& Var::operator=(const Var &other) {
	if (this == &other) return *this;
	if (other.refcount) XADD(other.refcount, 1);
	release();
	data = other.data;
	refcount = other.refcount;
	return *this;
}

SWORD_FORCEINLINE void Var::addref() { if (refcount) XADD(refcount, 1); }

SWORD_FORCEINLINE void Var::release() {
	if (refcount && XADD(refcount, -1) == 1) {
        delete data;
        delete refcount;
        // printf("Var Free!\n");
    }
	data = 0;
	refcount = 0;
}

SWORD_FORCEINLINE bool Var::operator<(const Var &other) const {
	return this->hash() < other.hash();
}

} // namespace sword

#endif // VAR_H
