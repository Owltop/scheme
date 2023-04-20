#pragma once

#include <memory>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int val) : value(val){};

    int GetValue() const {
        return value;
    };

    int value;
};

class Symbol : public Object {
public:
    Symbol(const std::string& input_s) : s(input_s){};

    const std::string& GetName() const {
        return s;
    };

    std::string s;
};

class Boolean : public Object {
public:
    Boolean(bool val) : value(val){};

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

    std::shared_ptr<Object> first = nullptr;
    std::shared_ptr<Object> second = nullptr;
};

// class NullList : public Object {};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

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
