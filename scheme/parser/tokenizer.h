#pragma once

#include "error.h"
#include <variant>
#include <optional>
#include <istream>
#include <unordered_set>
#include <cctype>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const;
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const;
};

struct DotToken {
    bool operator==(const DotToken&) const;
};

enum class BracketToken { OPEN, CLOSE };

struct BooleanToken {
    BooleanToken(bool val) : value(val){};

    bool operator==(const BooleanToken& other) const;

    explicit operator bool() {
        return value;
    }
    bool value;
};

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken& other) const;
};

using Token =
    std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken, BooleanToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in);

    bool IsEnd();

    void Next();

    Token GetToken();

    bool IsValid(char c);

    bool IsStartValid(char c);

protected:
    Token token;
    std::istream* istream;
    bool is_end = false;
};