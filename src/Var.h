#ifndef VAR_H
#define VAR_H

#include "SmartObj.h"
#include <string>

namespace sword {

struct VarBase {
    std::string name;
    int bits;
    int hash;
};

class SWORD_EXPORT Var : public SmartObj<VarBase> {
public:
	// empty
	Var();
    // instantiate
    Var(std::string name, int bits);
	// allocate
	void create();
    // print
    void print(FILE *f, bool detail = false) const;
    // get name
    const char *name() const;
    // get bits
    int bits() const;
    // get hash
    int hash() const;

	std::string str() const;

	bool operator<(const Var &other) const;
	bool eq(Var var) const;
};

SWORD_FORCEINLINE Var::Var() : SmartObj() {}

SWORD_FORCEINLINE Var::Var(std::string name, int bits) : SmartObj() {
	create();
	((VarBase*)data)->name = name;
    ((VarBase*)data)->bits = bits;
	((VarBase*)data)->hash = std::hash<std::string>()(name);
}

SWORD_FORCEINLINE bool Var::operator<(const Var &other) const {
	return this->hash() < other.hash();
}

} // namespace sword

#endif // VAR_H
