#ifndef EXPRESSIONTREE_H
#define EXPRESSIONTREE_H

#include "SmartObj.h"
#include <string>

namespace sword {

typedef enum {
    ID,
    INT,
    ADDSUB,
    MUL,
    ASSIGN,
    END,
    ENDFILE,
    UNKNOWN
} TokenSet;

struct ETBase {
    TokenSet token;
    std::string lexeme;
};

class SWORD_EXPORT ETNode : public SmartObj<ETBase> {
public:
    // emtpy
	ETNode();
	// instantiate
	ETNode(TokenSet token, std::string lexeme);
	// allocate
	void create() override;


};

SWORD_FORCEINLINE ETNode::ETNode() : SmartObj() {}

SWORD_FORCEINLINE ETNode::ETNode(TokenSet token, std::string lexeme) : SmartObj() {
	create();
    ((ETBase*)data)->token = token;
    ((ETBase*)data)->lexeme = lexeme;
}

class SWORD_EXPORT ETManager {
public:
    // instantiate
    ETManager();
    // parse
    int parse(const char *filepath);

private:
    void reset();
    TokenSet curToken;
    std::string lexeme;
};

SWORD_FORCEINLINE ETManager::ETManager() {
    curToken = UNKNOWN;
}

} // namespace sword

#endif // EXPRESSIONTREE_H