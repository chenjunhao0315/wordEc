#include "Var.h"

#include <unordered_map>

namespace sword {

void Var::create() {
	if (data) return;
	release();
	data = new VarBase;
	refcount = new int;
	*refcount = 1;
}

const char *Var::name() const {
	return ((VarBase*)data)->name.c_str();
}

int Var::bits() const {
	return ((VarBase*)data)->bits;
}

int Var::hash() const {
	return ((VarBase*)data)->hash;
}

void Var::print(FILE *f, bool detail) const {
	if (data) {
		if (detail)
			fprintf(f, "%s[%d:0]", name(), bits()-1);
		else
			fprintf(f, "%s", name());
	} else {
		fprintf(f, "Empty Var!");
	}
}

bool Var::eq(Var other) const {
	return this->data == other.data;
}

} // namespace sword
