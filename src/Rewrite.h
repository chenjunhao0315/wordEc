#ifndef REWRITE_H
#define REWRITE_H

#include "platform.h"
#include "PolyMgr.h"
#include <string>

namespace sword {

class SWORD_EXPORT RWManager {
public:
    RWManager() {}

    void createWord(std::string name, int inbit, int outbit);
    Polynomial getWord(std::string name);
    void parsePoly(std::string filepath);

    PolyMgr poly_man;
    std::vector<Polynomial> poly_vec;
    std::unordered_map<std::string, int> poly_pool;
}; 

} // namespace sword

#endif // REWRITE_H
