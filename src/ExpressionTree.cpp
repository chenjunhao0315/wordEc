#include "ExpressionTree.h"

#include <algorithm>

namespace sword {

// ETNode
void ETNode::create() {
    data = new ETBase;
	refcount = new int;
	*refcount = 1;
}

// ETManager
void ETManager::reset() {
    curToken = UNKNOWN;
    
}

} // namespace sword
