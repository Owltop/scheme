#include <tokenizer.h>

std::unordered_set<char> valid_start_symbols = {'[', '<', '=', '>', '*', '/', '#', ']'};
std::unordered_set<char> valid_symbols = {'[', '<', '=', '>', '*', '/', '#', '?', '!', '-', ']'};
char quote = char(39);

bool SymbolToken::operator==(const SymbolToken& other) const {
    return name == other.name;
};

bool QuoteToken::operator==(const QuoteToken&) const {
    return true;
};

bool DotToken::operator==(const DotToken&) const {
    return true;
};

bool BooleanToken::operator==(const BooleanToken& other) const {
    return value == other.value;
}

bool ConstantToken::operator==(const ConstantToken& other) const {
    return value == other.value;
};

bool Tokenizer::IsStartValid(char c) {  // + не считается валидным знаком??
    return std::isalpha(c) || std::isdigit(c) || valid_start_symbols.contains(c);
}

bool Tokenizer::IsValid(char c) {  // + не считается валидным знаком??
    return std::isalpha(c) || std::isdigit(c) || valid_symbols.contains(c);
}

Token Tokenizer::GetToken() {
    return token;
};

Tokenizer::Tokenizer(std::istream* in) {
    istream = in;
    Next();
};

bool Tokenizer::IsEnd() {
    return is_end;
};

void Tokenizer::Next() {
    char c = istream->get();
    while (c == ' ' || c == '\n') {
        c = istream->get();
        if (c == EOF) {
            is_end = true;
            return;
        }
    }

    if (c == EOF) {
        is_end = true;
        return;
    }

    if (c == quote) {
        token = QuoteToken();
    } else if ((c == '-' && std::isdigit(istream->peek()))  // число отрицательное
               || std::isdigit(c)) {                        // число положительное
        std::string raw_int(1, c);
        while (std::isdigit(istream->peek())) {
            c = istream->get();
            raw_int += c;
        }
        token = ConstantToken{.value = std::stoi(raw_int)};
    } else if (c == '(' || c == ')') {
        if (c == '(') {
            token = BracketToken::OPEN;
        } else {
            token = BracketToken::CLOSE;
        }
    } else if (c == '#' && (istream->peek() == 't' || istream->peek() == 'f')) {
        c = istream->get();
        if (c == 't') {
            token = BooleanToken(true);
        } else {
            token = BooleanToken(false);
        }
    } else if (c == '.') {
        token = DotToken();
    } else if (c == '+' && isdigit(istream->peek())) {
        c = istream->get();
        std::string raw_int(1, c);
        while (std::isdigit(istream->peek())) {
            c = istream->get();
            raw_int += c;
        }
        token = ConstantToken{.value = std::stoi(raw_int)};
        // token = SymbolToken{.name = "+"};
    } else if (c == '+') {
        token = SymbolToken{.name = "+"};
    } else if (c == '-') {
        token = SymbolToken{.name = "-"};
    } else {
        if (!IsStartValid(c)) {
            throw SyntaxError("Not valid symbol");
        }
        std::string for_token(1, c);
        while (istream->peek() != ' ' && istream->peek() != EOF && istream->peek() != '\n') {
            c = istream->get();
            if (!IsValid(c)) {
                throw SyntaxError("Not correct Symbol");
            }
            for_token += c;
        }
        token = SymbolToken{.name = for_token};
    }
};
