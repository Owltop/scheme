#pragma once

#include "error.h"
#include "tokenizer.h"

#include <algorithm>
#include <memory>
#include <limits>
#include <vector>
#include <sstream>
#include <map>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
    virtual std::string Serialize() = 0;
    virtual std::string QuoteSerialize() = 0;
};

class Function;

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {  //
    return std::dynamic_pointer_cast<T>(obj);
};

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    if (As<T>(obj) != nullptr) {
        return true;
    }
    return false;
};

class Number : public Object {
public:
    Number(int val) : value(val){};

    int GetValue() const {
        return value;
    };

    std::string Serialize() override {
        return std::to_string(value);
    }

    std::string QuoteSerialize() override {
        return std::to_string(value);
    }

    int value;
};

class Symbol : public Object {
public:
    Symbol(const std::string& input_s) : s(input_s){};

    const std::string& GetName() const {
        return s;
    };

    std::shared_ptr<Function> Eval();

    std::string Serialize() override {
        return s;
    }

    std::string QuoteSerialize() override {
        return s;
    }

    std::string s;
};

class Boolean : public Object {
public:
    Boolean(bool val) : value(val){};

    std::string Serialize() override {
        if (value) {
            return "#t";
        } else {
            return "#f";
        }
    }

    std::string QuoteSerialize() override {
        if (value) {
            return "#t";
        } else {
            return "#f";
        }
    }

    bool value = false;
};

class Cell : public Object {
public:
    std::shared_ptr<Object> GetFirst() const {
        return first;
    };
    std::shared_ptr<Object> GetSecond() const {
        return second;
    };

    std::string Serialize() override {
        std::string s1;
        std::string s2;
        std::string ans;
        if (first) {
            s1 = first->Serialize();
        }
        if (second) {
            s2 = second->Serialize();
        }
        if (flag_bracket) {  // если послднией селл был парой, то места для скобки не осталось
            s2 += ")";
        }
        if (s1 == "." && !s2.empty() && s2[0] == '(' && s2[s2.size() - 1] == ')') {
            ans = s2.substr(1, s2.size() - 2);
        } else if (!s1.empty() && !s2.empty() && s1 != "(" && s2 != ")") {
            ans = s1 + " " + s2;
        } else {
            ans = s1 + s2;
        }
        if (flag_bracket_for_list) {
            ans = "(" + ans + ")";
        }
        return ans;
    }

    std::string QuoteSerialize() override {
        std::string s1;
        std::string s2;
        std::string ans;
        if (first) {
            s1 = first->QuoteSerialize();
        }
        if (second) {
            s2 = second->QuoteSerialize();
        }
        if (Is<Number>(first) && Is<Number>(second)) {  // это пара
            ans = s1 + " . " + s2;
        } else {
            if (flag_bracket) {  // если послднией селл был парой, то места для скобки не осталось
                s2 += ")";
            }
            if (s1 == "." && !s2.empty() && s2[0] == '(' && s2[s2.size() - 1] == ')') {
                ans = s2.substr(1, s2.size() - 2);
            } else if (!s1.empty() && !s2.empty() && s1 != "(" && s2 != ")") {
                ans = s1 + " " + s2;
            } else {
                ans = s1 + s2;
            }
        }
        if (flag_bracket_for_list) {
            ans = "(" + ans + ")";
        }
        return ans;
    }

    std::shared_ptr<Object> first = nullptr;
    std::shared_ptr<Object> second = nullptr;
    bool flag_bracket = false;
    bool flag_bracket_for_list;
};

/*class SpecialCell : public Object {
public:
    std::shared_ptr<Object> GetFirst() const {
        return first;
    };
    std::shared_ptr<Object> GetSecond() const {
        return second;
    };

    std::string Serialize() override {
        std::string s1;
        std::string s2;
        if (first) {
            s1 = first->Serialize();
        }
        if (second) {
            s2 = second->Serialize();
        }
        if (flag_bracket) {  // если послднией селл был парой, то места для скобки не осталось
            s2 += ")";
        }
        if (s1 == "." && !s2.empty() && s2[0] == '(' && s2[s2.size() - 1] == ')') {
            return "(" + s2.substr(1, s2.size() - 2) + ")";
        }
        if (!s1.empty() && !s2.empty() && s1 != "(" && s2 != ")") {
            return "(" + s1 + " " + s2 + ")";
        } else {
            return "(" + s1 + s2 + ")";
        }
    }

    std::string QuoteSerialize() override {
        std::string s1;
        std::string s2;
        if (first) {
            s1 = first->QuoteSerialize();
        }
        if (second) {
            s2 = second->QuoteSerialize();
        }
        if (Is<Number>(first) && Is<Number>(second)) {  // это пара
            return "(" + s1 + " . " + s2 + ")";
        }
        if (flag_bracket) {  // если послднией селл был парой, то места для скобки не осталось
            s2 += ")";
        }
        if (s1 == "." && !s2.empty() && s2[0] == '(' && s2[s2.size() - 1] == ')') {
            return "(" + s2.substr(1, s2.size() - 2) + ")";
        }
        if (!s1.empty() && !s2.empty() && s1 != "(" && s2 != ")") {
            return "(" + s1 + " " + s2 + ")";
        } else {
            return "(" + s1 + s2 + ")";
        }
    }

    std::shared_ptr<Object> first = nullptr;
    std::shared_ptr<Object> second = nullptr;
    bool flag_bracket = false;
};*/

class Quote : public Object {
public:
    Quote(const std::string& input_s) : s(input_s){};

    std::string Serialize() override {
        return s;
    }

    std::string QuoteSerialize() override {
        return s;
    }

    std::string s;
};

class Function : public Object {
public:
    virtual std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) = 0;

    std::string Serialize() override {
        throw RuntimeError("Serialize should not work for Function");
    }

    std::string QuoteSerialize() override {
        throw RuntimeError("Serialize should not work for Function");
    }
};

namespace tools {
std::shared_ptr<Object> MyRead(Tokenizer* tokenizer);
std::shared_ptr<Object> MyReadList(Tokenizer* tokenizer);
std::shared_ptr<Object> MyQuoteRead(Tokenizer* tokenizer);
std::shared_ptr<Object> MyQuoteReadList(Tokenizer* tokenizer);
std::shared_ptr<Object> QuoteToAst(std::shared_ptr<Object> raw_object);
bool IsList(std::shared_ptr<Object> object);
bool ChangeLastElement(std::shared_ptr<Object> object, std::string, bool);
}  // namespace tools

class NumberQuestion : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            throw SyntaxError("number?");
        }
        if (Is<Number>(args[0])) {
            return std::make_shared<Boolean>(true);
        } else {
            return std::make_shared<Boolean>(false);
        }
    }
};

class Equality : public Function {
    std::shared_ptr<Object> Apply(
        std::vector<std::shared_ptr<Object>>& args) override {  // может быть больше 2 аргументов
        if (args.empty()) {
            return std::make_shared<Boolean>(true);
        }
        if (args.size() == 1 || !Is<Number>(args[0])) {
            throw RuntimeError("1 arg after = or not number");
        }
        for (size_t i = 1; i < args.size(); ++i) {
            if (!Is<Number>(args[i])) {
                throw RuntimeError("not number");
            }
            if (As<Number>(args[i])->GetValue() != As<Number>(args[i - 1])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
};

class More : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            return std::make_shared<Boolean>(true);
        }
        if (args.size() == 1 || !Is<Number>(args[0])) {
            throw RuntimeError("1 arg after = or not number");
        }
        for (size_t i = 1; i < args.size(); ++i) {
            if (!Is<Number>(args[i])) {
                throw RuntimeError("not number");
            }
            if (!(As<Number>(args[i])->GetValue() < As<Number>(args[i - 1])->GetValue())) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
};

class Less : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            return std::make_shared<Boolean>(true);
        }
        if (args.size() == 1 || !Is<Number>(args[0])) {
            throw RuntimeError("1 arg after = or not number");
        }
        for (size_t i = 1; i < args.size(); ++i) {
            if (!Is<Number>(args[i])) {
                throw RuntimeError("not number");
            }
            if (!(As<Number>(args[i])->GetValue() > As<Number>(args[i - 1])->GetValue())) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
};

class MoreEq : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            return std::make_shared<Boolean>(true);
        }
        if (args.size() == 1 || !Is<Number>(args[0])) {
            throw RuntimeError("1 arg after = or not number");
        }
        for (size_t i = 1; i < args.size(); ++i) {
            if (!Is<Number>(args[i])) {
                throw RuntimeError("not number");
            }
            if (!(As<Number>(args[i])->GetValue() <= As<Number>(args[i - 1])->GetValue())) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
};

class LessEq : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            return std::make_shared<Boolean>(true);
        }
        if (args.size() == 1 || !Is<Number>(args[0])) {
            throw RuntimeError("1 arg after = or not number");
        }
        for (size_t i = 1; i < args.size(); ++i) {
            if (!Is<Number>(args[i])) {
                throw RuntimeError("not number");
            }
            if (!(As<Number>(args[i])->GetValue() >= As<Number>(args[i - 1])->GetValue())) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
};

class Plus : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            return std::make_shared<Number>(0);
        }
        int result = 0;
        for (size_t i = 0; i < args.size(); ++i) {
            if (!Is<Number>(args[i])) {
                throw RuntimeError("not number");
            }
            result += As<Number>(args[i])->GetValue();
        }
        return std::make_shared<Number>(result);
    }
};

class Minus : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.size() <= 1) {
            throw RuntimeError("1 sym after +");
        }
        int result = 0;
        for (size_t i = 0; i < args.size(); ++i) {
            if (!Is<Number>(args[i])) {
                throw RuntimeError("not number");
            }
            if (i == 0) {
                result = As<Number>(args[i])->GetValue();
            } else {
                result -= As<Number>(args[i])->GetValue();
            }
        }
        return std::make_shared<Number>(result);
    }
};

class Multiplication : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            return std::make_shared<Number>(1);
        }
        int result = 1;
        for (size_t i = 0; i < args.size(); ++i) {
            if (!Is<Number>(args[i])) {
                throw RuntimeError("not number");
            }
            result *= As<Number>(args[i])->GetValue();
        }
        return std::make_shared<Number>(result);
    }
};

class Division : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.size() <= 1) {
            throw RuntimeError("1 sym after +");
        }
        int result = 1;
        for (size_t i = 0; i < args.size(); ++i) {
            if (!Is<Number>(args[i])) {
                throw RuntimeError("not number");
            }
            if (i == 0) {
                result = As<Number>(args[i])->GetValue();
            } else {
                result /= As<Number>(args[i])->GetValue();
            }
        }
        return std::make_shared<Number>(result);
    }
};

class Max : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            throw RuntimeError("no args with max");
        }
        int result = -2147483648;
        for (size_t i = 0; i < args.size(); ++i) {
            if (!Is<Number>(args[i])) {
                throw RuntimeError("not number");
            }
            result = std::max(result, As<Number>(args[i])->GetValue());
        }
        return std::make_shared<Number>(result);
    }
};

class Min : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            throw RuntimeError("no args with min");
        }
        int result = 2147483647;
        for (size_t i = 0; i < args.size(); ++i) {
            if (!Is<Number>(args[i])) {
                throw RuntimeError("not number");
            }
            result = std::min(result, As<Number>(args[i])->GetValue());
        }
        return std::make_shared<Number>(result);
    }
};

class Abs : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.size() != 1) {
            throw RuntimeError("args with abs");
        }
        if (!Is<Number>(args[0])) {
            throw RuntimeError("not number abs");
        }
        return std::make_shared<Number>(abs(As<Number>(args[0])->GetValue()));
    }
};

class BooleanQuestion : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            throw SyntaxError("shit in boolean?");
        }
        if (Is<Boolean>(args[0])) {
            return std::make_shared<Boolean>(true);
        } else {
            return std::make_shared<Boolean>(false);
        }
    }
};

class Not : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.size() != 1) {
            throw RuntimeError("shit in not");
        }
        if (Is<Boolean>(args[0])) {
            if (As<Boolean>(args[0])->value) {
                return std::make_shared<Boolean>(false);
            } else {
                return std::make_shared<Boolean>(true);
            }
        }
        return std::make_shared<Boolean>(false);
    }
};

class And : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            return std::make_shared<Boolean>(true);
        }
        std::shared_ptr<Object> result = nullptr;
        for (size_t i = 0; i < args.size(); ++i) {
            if (Is<Boolean>(args[i])) {
                if (As<Boolean>(args[i])->value == false) {
                    result = args[i];
                    return result;
                }
            } else {
                result = args[i];
            }
        }
        return args[args.size() - 1];
    }
};

class Or : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            return std::make_shared<Boolean>(false);
        }
        std::shared_ptr<Object> result = nullptr;
        for (size_t i = 0; i < args.size(); ++i) {
            if (Is<Boolean>(args[i])) {
                if (As<Boolean>(args[i])->value == true) {
                    result = args[i];
                    return result;
                }
            } else {
                result = args[i];
                return result;
            }
        }
        return args[args.size() - 1];
    }
};

class PairQuestion : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            return std::make_shared<Boolean>(false);
        }
        if (!Is<Quote>(args[0])) {
            return std::make_shared<Boolean>(false);
        }

        std::shared_ptr<Object> input_ast = tools::QuoteToAst(args[0]);
        if (Is<Cell>(input_ast)) {
            auto obj = As<Cell>(input_ast);
            if (!Is<Cell>(obj->GetFirst()) && !Is<Cell>(obj->GetSecond())) {
                return std::make_shared<Boolean>(true);
            } else if (!Is<Cell>(obj->GetFirst()) && Is<Cell>(obj->GetSecond()) &&
                       !Is<Cell>(As<Cell>(obj->GetSecond())->GetFirst())) {
                return std::make_shared<Boolean>(true);
            }
        }
        return std::make_shared<Boolean>(false);
    }
};

class NullQuestion : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            return std::make_shared<Boolean>(false);
        }
        if (!Is<Quote>(args[0])) {
            return std::make_shared<Boolean>(false);
        }

        std::shared_ptr<Object> input_ast = tools::QuoteToAst(args[0]);
        if (input_ast == nullptr) {
            return std::make_shared<Boolean>(true);
        } else {
            return std::make_shared<Boolean>(false);
        }
    }
};

class ListQuestion : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            return std::make_shared<Boolean>(false);
        }
        if (!Is<Quote>(args[0])) {
            return std::make_shared<Boolean>(false);
        }

        std::shared_ptr<Object> input_ast = tools::QuoteToAst(args[0]);
        return std::make_shared<Boolean>(tools::IsList(input_ast));
    }
};

class Cons : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        // Здесь говёный код, мб надо переписать
        if (args.size() != 2) {
            throw RuntimeError("Cons shit");
        }
        auto ans = std::make_shared<Cell>();
        ans->first = args[0];
        ans->second = std::make_shared<Cell>();
        As<Cell>(ans->second)->first = std::make_shared<Symbol>(".");
        As<Cell>(ans->second)->second = std::make_shared<Cell>();
        As<Cell>(As<Cell>(ans->second)->second)->first = args[1];
        As<Cell>(As<Cell>(ans->second)->second)->second = std::make_shared<Symbol>(")");
        auto real_ans = std::make_shared<Cell>();
        As<Cell>(real_ans)->first = std::make_shared<Symbol>("(");
        As<Cell>(real_ans)->second = ans;
        return real_ans;
    }
};

class Car : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            throw RuntimeError("Car");
        }
        if (!Is<Quote>(args[0])) {
            throw RuntimeError("Car");
        }

        std::shared_ptr<Object> input_ast = tools::QuoteToAst(args[0]);
        if (Is<Cell>(input_ast)) {
            return As<Cell>(input_ast)->GetFirst();
        } else {
            return input_ast;
        }
    }
};

class Cdr : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        // не очень хороший код
        if (args.empty()) {
            throw RuntimeError("Cdr");
        }
        if (!Is<Quote>(args[0])) {
            throw RuntimeError("Cdr");
        }

        /* std::string s = As<Quote>(args[0])->s;
         size_t i = 0;
         while (s[i] != ' ' && s[i] != EOF && s[i] != '\n') {
             ++i;
             if (i >= s.size()) {
                 return std::make_shared<Symbol>("()");
             }
         }
         while (s[i] == ' ' || s[i] == EOF || s[i] == '\n' || s[i] == '.') {
             ++i;
             if (i >= s.size()) {
                 return std::make_shared<Symbol>("()");
             }
         }
         s = "(" + s.substr(i, s.size() - i);
         if (std::find(s.begin(), s.end(), ' ') == s.end()) {
             s = s.substr(1, s.size() - 2);
         }
         return std::make_shared<Symbol>(s);*/
        // Проблема в том что QuoteToAst проглатывает точки, из-за чего неудобно работать
        std::shared_ptr<Object> input_ast = tools::QuoteToAst(args[0]);
        bool flag = false;
        if (Is<Cell>(input_ast)) {
            if (Is<Cell>(As<Cell>(input_ast)->GetSecond())) {  // это список
                flag = true;
            }
            As<Cell>(input_ast)->first = nullptr;
            auto s = input_ast->QuoteSerialize();
            if (s.empty()) {
                return std::make_shared<Symbol>("()");
            }
            if (flag) {
                return std::make_shared<Symbol>("(" + s + ")");  // здесь говно
            } else {
                return std::make_shared<Symbol>(s);
            }
        } else {
            throw RuntimeError("Cdr");
        }
    }
};

class ListConstruct : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        std::shared_ptr<Object> ans = std::make_shared<Cell>();
        auto current = ans;
        for (size_t i = 0; i < args.size(); ++i) {
            As<Cell>(current)->first = args[i];
            As<Cell>(current)->second = std::make_shared<Cell>();
            current = As<Cell>(current)->second;
        }
        As<Cell>(ans)->flag_bracket_for_list = true;
        return ans;
    }
};

class ListRef : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.size() != 2) {
            throw RuntimeError("ListRef");
        }
        if (!Is<Number>(args[1]) || !Is<Quote>(args[0])) {
            throw RuntimeError("ListRef");
        }
        size_t num = As<Number>(args[1])->GetValue();
        std::shared_ptr<Object> input_ast = tools::QuoteToAst(args[0]);
        size_t i = 0;
        std::shared_ptr<Object> ans;
        auto current = input_ast;
        while (true) {
            if (Is<Cell>(current)) {
                if (As<Cell>(current)->GetFirst() != nullptr) {
                    if (i == num) {
                        return As<Cell>(current)->GetFirst();
                    }
                    ++i;
                    current = As<Cell>(current)->GetSecond();
                }
            } else if (current != nullptr) {
                if (i == num) {
                    return current;
                }
                ++i;
            } else {
                break;
            }
        }
        throw RuntimeError("ListRef");
    }
};

class ListTail : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>>& args) override {
        if (args.size() != 2) {
            throw RuntimeError("ListRef");
        }
        if (!Is<Number>(args[1]) || !Is<Quote>(args[0])) {
            throw RuntimeError("ListRef");
        }
        size_t num = As<Number>(args[1])->GetValue();
        std::shared_ptr<Object> input_ast = tools::QuoteToAst(args[0]);
        size_t i = 0;
        std::shared_ptr<Object> ans;
        auto current = input_ast;
        while (true) {
            if (Is<Cell>(current)) {
                if (As<Cell>(current)->GetFirst() != nullptr) {
                    if (i + 1 == num) {
                        ans = As<Cell>(current)->GetSecond();
                        break;
                    }
                    ++i;
                    current = As<Cell>(current)->GetSecond();
                }
            } else if (current != nullptr) {
                if (i + 1 == num) {  // заходит ли сюда программа?
                    // return current;
                    ans = std::make_shared<Cell>();
                    break;
                }
            } else {
                break;
            }
        }
        if (i + 1 == num) {
            if (ans == nullptr) {
                ans = std::make_shared<Cell>();
            }
            As<Cell>(ans)->flag_bracket_for_list = true;
            return ans;
        }
        throw RuntimeError("ListTail");
    }
};
