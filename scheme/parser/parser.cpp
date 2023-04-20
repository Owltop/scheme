#include <parser.h>

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("tokenizer is empty");
        // return nullptr;
    }
    auto token = tokenizer->GetToken();
    tokenizer->Next();
    if (auto tt = std::get_if<BracketToken>(&token)) {
        if (*tt == BracketToken::OPEN) {
            return ReadList(tokenizer);
        } else {
            return std::make_shared<Symbol>(")");
        }
    } else {
        if (auto tt = std::get_if<SymbolToken>(&token)) {
            return std::make_shared<Symbol>(tt->name);
        } else if (auto tt = std::get_if<ConstantToken>(&token)) {
            return std::make_shared<Number>(tt->value);
        } else if (auto tt = std::get_if<QuoteToken>(&token)) {
            throw SyntaxError("quote token");
        } else if (auto tt = std::get_if<DotToken>(&token)) {
            return std::make_shared<Symbol>(".");
        } else if (auto tt = std::get_if<BooleanToken>(&token)) {
            if (tt->value) {
                return std::make_shared<Boolean>(true);
            } else {
                return std::make_shared<Boolean>(false);
            }
        } else {
            throw RuntimeError("42");
        }
    }
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    std::shared_ptr<Cell> head = std::make_shared<Cell>();
    std::shared_ptr<Cell> tail = head;
    bool was_dot = false;
    bool was_something = false;
    bool was_something_after_dot = false;
    while (!tokenizer->IsEnd()) {
        auto raw_object = Read(tokenizer);
        if (Is<Symbol>(raw_object)) {
            auto object = As<Symbol>(raw_object);
            if (object->GetName() == ")") {
                if (was_dot && !was_something_after_dot) {
                    throw SyntaxError("no symbols after dot");
                }
                if (!was_something) {
                    return nullptr;
                } else {
                    return head;
                }
            } else if (object->GetName() == ".") {
                was_something = true;
                was_dot = true;
            } else {
                was_something = true;
                if (was_dot) {
                    was_something_after_dot = true;
                }
                if (head->first == nullptr) {
                    head->first = object;
                } else {
                    if (was_dot) {
                        tail->second = object;
                    } else {
                        auto new_tail = std::make_shared<Cell>();
                        new_tail->first = object;
                        tail = new_tail;
                    }
                }
            }
        } else {
            was_something = true;
            if (was_dot) {
                was_something_after_dot = true;
            }
            if (head->first == nullptr) {
                head->first = raw_object;
            } else {
                if (was_dot) {
                    if (tail->second == nullptr) {
                        tail->second = raw_object;
                    } else {
                        throw SyntaxError("more than 1 symbol after dot");
                    }
                } else {
                    auto new_tail = std::make_shared<Cell>();
                    new_tail->first = raw_object;
                    tail->second = new_tail;
                    tail = new_tail;
                }
            }
        }
    }
    throw SyntaxError("no close bracket");
}