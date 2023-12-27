#ifndef SMARTOBJ_H
#define SMARTOBJ_H

#include "platform.h"
#include "allocator.h"
#include <stdio.h>

namespace sword {

template <typename T>
class SWORD_EXPORT SmartObj {
public:
    // empty
	SmartObj();
	// copy
	SmartObj(const SmartObj &other);
	// release
	virtual ~SmartObj();
	// assign
	virtual SmartObj& operator=(const SmartObj& other);
	// allocate
	virtual void create() = 0;
	// refcount++
	virtual void addref();
	// refcount--
	virtual void release();
	// empty
	virtual bool empty() const;

    T    *data;
    int  *refcount;
};

template <typename T>
SWORD_FORCEINLINE SmartObj<T>::SmartObj() : data(0), refcount(0) {}

template <typename T>
SWORD_FORCEINLINE SmartObj<T>::SmartObj(const SmartObj &other) : data(other.data), refcount(other.refcount) { addref(); }

template <typename T>
SWORD_FORCEINLINE SmartObj<T>::~SmartObj() { release(); }

template <typename T>
SWORD_FORCEINLINE SmartObj<T>& SmartObj<T>::operator=(const SmartObj<T> &other) {
	if (this == &other) return *this;
	if (other.refcount) XADD(other.refcount, 1);
	release();
	data = other.data;
	refcount = other.refcount;
	return *this;
}

template <typename T>
SWORD_FORCEINLINE void SmartObj<T>::addref() { if (refcount) XADD(refcount, 1); }

template <typename T>
SWORD_FORCEINLINE void SmartObj<T>::release() {
	if (refcount && XADD(refcount, -1) == 1) {
		delete data;
		delete refcount;
	}
	data = 0;
	refcount = 0;
}

template <typename T>
SWORD_FORCEINLINE bool SmartObj<T>::empty() const {
	return this->data == 0;
}

} // namespace sword

#endif // SMARTOBJ_H