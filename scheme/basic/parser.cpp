#include "parser.h"

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    return tools::MyRead(tokenizer);
};