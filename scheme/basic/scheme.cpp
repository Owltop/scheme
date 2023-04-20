#include "scheme.h"

std::shared_ptr<Object> ForAst(std::shared_ptr<Object>);

std::vector<std::shared_ptr<Object>> ConvertArgsToVector(std::shared_ptr<Object> object) {
    std::vector<std::shared_ptr<Object>> ans;
    if (!object) {
        return ans;
    }

    if (Is<Cell>(object)) {
        try {
            std::shared_ptr<Object> first;
            if (Is<Cell>(As<Cell>(object)->GetFirst())) {
                first = ForAst(As<Cell>(object)->GetFirst());
            } else {
                first = As<Cell>(object)->GetFirst();
            }
            std::vector<std::shared_ptr<Object>> v1 = ConvertArgsToVector(first);
            std::vector<std::shared_ptr<Object>> v2 =
                ConvertArgsToVector(As<Cell>(object)->GetSecond());
            v1.reserve(v1.size() + v2.size() + 1);
            v1.insert(v1.end(), v2.begin(), v2.end());
            return v1;
        } catch (...) {
            ans.push_back(object);
        }
    } else {
        ans.push_back(object);
    }
    return ans;
}

std::string Interpreter::Run(const std::string& str) {  // этот код с семинара Тагира
    try {
        std::stringstream ss{str};
        Tokenizer tokenizer{&ss};

        std::shared_ptr<Object> input_ast = tools::MyRead(&tokenizer);
        if (!tokenizer.IsEnd()) {
            throw SyntaxError("42");
        }

        if (input_ast == nullptr) {
            throw RuntimeError("42");
        } else if (Is<Cell>(input_ast)) {
            if (As<Cell>(input_ast)->GetFirst() == nullptr &&
                As<Cell>(input_ast)->GetSecond() == nullptr) {
                throw RuntimeError("42");
            }
        }

        std::shared_ptr<Object> output_ast = ForAst(input_ast);

        if (output_ast) {
            if (Is<Cell>(output_ast)) {
                return output_ast->Serialize();  //
            } else {
                return output_ast->Serialize();
            }
        } else {
            throw RuntimeError("output_ast == nullptr");
        }
    } catch (RuntimeError) {
        throw;
    } catch (SyntaxError) {
        throw;
    } catch (...) {
        throw RuntimeError("unexpected error");
    }
}

std::shared_ptr<Object> ForAst(std::shared_ptr<Object> input_ast) {
    if (input_ast == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Object> output_ast = nullptr;
    if (Is<Cell>(input_ast)) {
        auto first = As<Cell>(input_ast)->GetFirst();
        auto second = As<Cell>(input_ast)->GetSecond();
        if (Is<Symbol>(first)) {  // это функция
            auto function = As<Symbol>(first)->Eval();
            if (Is<Cell>(second)) {
                if (As<Cell>(second)->GetFirst() == nullptr &&
                    As<Cell>(second)->GetSecond() == nullptr) {
                    throw RuntimeError("42");
                }
            }
            std::vector<std::shared_ptr<Object>> args = ConvertArgsToVector(second);
            output_ast = function->Apply(args);
        } else if (Is<Quote>(first)) {  // это цитата
            // throw RuntimeError("42");
            auto new_first = ForAst(first);
            auto new_second = ForAst(second);
            output_ast = std::make_shared<Cell>();
            As<Cell>(output_ast)->first = new_first;
            As<Cell>(output_ast)->second = new_second;
        } else {
            throw RuntimeError("42");
        }
    } else if (Is<Number>(input_ast)) {
        output_ast = std::make_shared<Number>(As<Number>(input_ast)->GetValue());
    } else if (Is<Boolean>(input_ast)) {
        output_ast = std::make_shared<Boolean>(As<Boolean>(input_ast)->value);
    } else if (Is<Quote>(input_ast)) {
        output_ast = std::make_shared<Quote>(As<Quote>(input_ast)->Serialize());
    }

    /*

    auto output_ast = function->Apply(args);

    return output_ast->Serialize();*/

    return output_ast;
}
