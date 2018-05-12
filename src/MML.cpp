#include <ctype.h>

#include "MML.h"

void MMLParserTokenizer::handleToken(MMLToken token) {
	// printf("MMLParserTokenizer token=%s\n", token.getTypeName(token.type));
	parent->handleToken(token);
}
