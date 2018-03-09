#pragma once

#include <functional>
#include <utility>
#include <type_traits>
#include <iostream>

#include "ParseError.hpp"

namespace Interpreter {

struct Epsilon {
    template <size_t = 0>
    static bool MatchToken(const Token&, const std::function<void()>&) {
        return true;
    }

    template <size_t Index = 0>
    static constexpr std::vector<Token::Type> ExpectedToken() {
        return { Token::Type::Invalid };
    }
};

template <Token::Type Type>
struct TokenType {
    template <size_t = 0>
    static bool MatchToken(Token token) {
        return token.type == Type;
    }

    static void RequireToken(Token token) {
        if (token.type != Type)
            throw ParseError(token, { Type });
    }

    template <size_t Index = 0>
    static constexpr std::vector<Token::Type> ExpectedToken() {
        return { Type };
    }
};

template <typename T>
struct List {
    using Type = std::decay_t<T>;

    template <size_t = 0>
    static bool MatchToken(Token token) {
        return Type::MatchToken(token);
    }

    template <size_t Index = 0>
    static constexpr std::vector<Token::Type> ExpectedToken() {
        return Type::ExpectedToken();
    }
};

template <typename... Args>
struct Rule {
    template <size_t Index>
    using Type = std::decay_t<std::tuple_element_t<Index, std::tuple<Args...>>>;
    static constexpr size_t Size = sizeof...(Args);

    template <size_t Index = 0>
    static bool MatchToken(Token token) {
        return Type<Index>::MatchToken(token);
    }

    template <size_t Index = 0>
    static constexpr std::vector<Token::Type> ExpectedToken() {
        return Type<Index>::ExpectedToken();
    }
};

template <typename... Args>
struct RuleGroup {
    template <size_t Index>
    using Type = std::decay_t<std::tuple_element_t<Index, std::tuple<Args...>>>;
    static constexpr size_t Size = sizeof...(Args);

    template <size_t Index = 0>
    static bool MatchToken(Token token) {
        if (Type<Index>::MatchToken(token))
            return true;
        if constexpr (Index + 1 < Size)
            return MatchToken<Index + 1>(token);
        return false;
    }

    template <size_t Index = 0>
    static constexpr std::vector<Token::Type> ExpectedToken() {
        auto head = Type<Index>::ExpectedToken();
        if constexpr (Index + 1 < Size) {
            auto tail = Type<Index + 1>::ExpectedToken();
            head.reserve(head.size() + tail.size());
            head.insert(head.end(), tail.begin(), tail.end());
        }
        return head;
    }
};
}