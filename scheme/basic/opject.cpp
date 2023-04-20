#include "object.h"

std::shared_ptr<Function> Symbol::Eval() {
    static std::map<std::string, std::shared_ptr<Function>> functions = {
        {"number?", std::make_shared<NumberQuestion>()},
        {"=", std::make_shared<Equality>()},
        {">", std::make_shared<More>()},
        {"<", std::make_shared<Less>()},
        {">=", std::make_shared<MoreEq>()},
        {"<=", std::make_shared<LessEq>()},
        {"+", std::make_shared<Plus>()},
        {"-", std::make_shared<Minus>()},
        {"*", std::make_shared<Multiplication>()},
        {"/", std::make_shared<Division>()},
        {"max", std::make_shared<Max>()},
        {"min", std::make_shared<Min>()},
        {"abs", std::make_shared<Abs>()},
        {"boolean?", std::make_shared<BooleanQuestion>()},
        {"not", std::make_shared<Not>()},
        {"and", std::make_shared<And>()},
        {"or", std::make_shared<Or>()},
        {"pair?", std::make_shared<PairQuestion>()},
        {"null?", std::make_shared<NullQuestion>()},
        {"list?", std::make_shared<ListQuestion>()},
        {"cons", std::make_shared<Cons>()},
        {"car", std::make_shared<Car>()},
        {"cdr", std::make_shared<Cdr>()},
        {"list", std::make_shared<ListConstruct>()},
        {"list-ref", std::make_shared<ListRef>()},
        {"list-tail", std::make_shared<ListTail>()}};
    if (functions.contains(s)) {
        return functions[s];
    }
    throw RuntimeError("shit in Eval");
}

namespace tools {
std::shared_ptr<Object> MyRead(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("tokenizer is empty or nothing after quote");
        // return nullptr;
    }
    auto token = tokenizer->GetToken();
    tokenizer->Next();
    if (auto tt = std::get_if<BracketToken>(&token)) {
        if (*tt == BracketToken::OPEN) {
            return MyReadList(tokenizer);
        } else {
            return std::make_shared<Symbol>(")");
        }
    } else {
        if (auto tt = std::get_if<SymbolToken>(&token)) {
            if (tt->name == "quote") {
                std::shared_ptr<Object> next_object = MyQuoteRead(tokenizer);
                return std::make_shared<Quote>(next_object->Serialize());
            }
            return std::make_shared<Symbol>(tt->name);
        } else if (auto tt = std::get_if<ConstantToken>(&token)) {
            return std::make_shared<Number>(tt->value);
        } else if (auto tt = std::get_if<QuoteToken>(&token)) {
            std::shared_ptr<Object> next_object = MyQuoteRead(tokenizer);
            if (next_object == nullptr) {
                throw RuntimeError("42");
            }
            /*if (Is<Cell>(next_object)) {
                return std::make_shared<Quote>("(" + next_object->Serialize() + ")");
            } else {
                return std::make_shared<Quote>(next_object->Serialize());
            }*/
            return std::make_shared<Quote>(next_object->Serialize());
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

std::shared_ptr<Object> MyReadList(Tokenizer* tokenizer) {
    std::shared_ptr<Cell> head = std::make_shared<Cell>();
    std::shared_ptr<Cell> tail = head;
    bool was_dot = false;
    bool was_something = false;
    bool was_something_after_dot = false;
    while (!tokenizer->IsEnd()) {
        auto raw_object = MyRead(tokenizer);
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
                if (!was_something) {
                    throw SyntaxError("42");
                }
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
                        tail->second = new_tail;  // этого не было
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

std::shared_ptr<Object> MyQuoteRead(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("tokenizer is empty or nothing after quote");
        // return nullptr;
    }
    auto token = tokenizer->GetToken();
    tokenizer->Next();
    if (auto tt = std::get_if<BracketToken>(&token)) {
        if (*tt == BracketToken::OPEN) {
            std::shared_ptr<Object> new_p = std::make_shared<Cell>();
            As<Cell>(new_p)->first = std::make_shared<Symbol>("(");
            As<Cell>(new_p)->second = MyQuoteReadList(tokenizer);
            return new_p;
        } else {
            return std::make_shared<Symbol>(")");
        }
    } else {
        if (auto tt = std::get_if<SymbolToken>(&token)) {
            if (tt->name == "quote") {
                std::shared_ptr<Object> next_object = MyQuoteRead(tokenizer);
                return std::make_shared<Quote>(next_object->Serialize());
            }
            return std::make_shared<Symbol>(tt->name);
        } else if (auto tt = std::get_if<ConstantToken>(&token)) {
            return std::make_shared<Number>(tt->value);
        } else if (auto tt = std::get_if<QuoteToken>(&token)) {
            std::shared_ptr<Object> next_object = MyQuoteRead(tokenizer);
            if (next_object == nullptr) {
                throw RuntimeError("42");
            }
            /*if (Is<Cell>(next_object)) {
                return std::make_shared<Quote>("(" + next_object->Serialize() + ")");
            } else {
                return std::make_shared<Quote>(next_object->Serialize());
            }*/
            return std::make_shared<Quote>(next_object->Serialize());
        } else if (auto tt = std::get_if<DotToken>(&token)) {
            return std::make_shared<Symbol>(".");
        } else if (auto tt = std::get_if<BooleanToken>(&token)) {
            if (tt->value) {
                return std::make_shared<Boolean>(true);
            } else {
                return std::make_shared<Boolean>(false);
            }
        } else {
            // throw SyntaxError("sdcer");
            throw RuntimeError("42");
        }
    }
}

std::shared_ptr<Object> MyQuoteReadList(Tokenizer* tokenizer) {
    std::shared_ptr<Cell> head = std::make_shared<Cell>();
    std::shared_ptr<Cell> tail = head;
    bool was_dot = false;
    bool was_something = false;
    bool was_something_after_dot = false;
    while (!tokenizer->IsEnd()) {
        auto raw_object = MyQuoteRead(tokenizer);
        if (Is<Symbol>(raw_object)) {
            auto object = As<Symbol>(raw_object);
            if (object->GetName() == ")") {
                if (was_dot && !was_something_after_dot) {
                    throw SyntaxError("no symbols after dot");
                } else {
                    if (was_dot) {
                        if (tail->second != nullptr) {
                            As<Cell>(head)->flag_bracket = true;
                            return head;
                        } else {
                            auto new_tail = std::make_shared<Cell>();
                            new_tail->first = raw_object;
                            tail->second = new_tail;
                            return head;
                        }
                    } else {
                        auto new_tail = std::make_shared<Cell>();
                        new_tail->first = raw_object;
                        tail->second = new_tail;
                        return head;
                    }
                }
            } else if (object->GetName() == ".") {
                if (!was_something) {
                    throw SyntaxError("42");
                }
                was_something = true;
                was_dot = true;
                auto new_tail = std::make_shared<Cell>();
                new_tail->first = object;
                tail->second = new_tail;
                tail = new_tail;
                /*tail->second = MyRead(tokenizer);
                return head;*/
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
                        new_tail->first = raw_object;
                        tail->second = new_tail;
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

std::shared_ptr<Object> QuoteToAst(std::shared_ptr<Object> raw_object) {
    if (!Is<Quote>(raw_object)) {
        throw RuntimeError("tools");
    }
    auto object = As<Quote>(raw_object);
    std::stringstream ss{object->s};
    Tokenizer tokenizer{&ss};

    return MyRead(&tokenizer);
}

bool IsList(std::shared_ptr<Object> object) {
    if (object == nullptr) {
        return true;
    }
    if (!Is<Cell>(object)) {
        return false;
    }
    if (!Is<Cell>((As<Cell>(object)->GetFirst())) && IsList(As<Cell>(object)->GetSecond())) {
        return true;
    }
    return false;
}

bool ChangeLastElement(std::shared_ptr<Object> object, std::string to = ")", bool flag = false) {
    if (Is<Cell>(object)) {
        if (As<Cell>(object)->GetSecond() != nullptr) {
            ChangeLastElement(As<Cell>(object)->GetSecond(), to);
            return true;
        } else {
            As<Cell>(object)->GetFirst() = std::make_shared<Symbol>(to);
            return true;
        }
    } else if (!flag) {
        object = std::make_shared<Symbol>(to);
    } else {
        return false;
    }
    return false;
}
}  // namespace tools