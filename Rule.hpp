#pragma once

#include <functional>
#include <stdexcept>
#include <tuple>
#include <variant>
#include <utility>
#include <type_traits>

#include <typeinfo>
#include <iostream>

namespace Interpreter {
namespace Grammar {

template <Token::Type t>
struct TokenType {
    template <size_t = 0>
    static bool MatchToken(Token token, const std::function<Token()>&) {
        std::cout << "Got " << token << ", expected" << t << std::endl;
        return token.type == t;
    }
};

template <typename... Args>
struct Rule : public std::tuple<Args...> {
    using base = std::tuple<Args...>;
    using base::base;
    static constexpr size_t tuple_size = sizeof...(Args);
    template <size_t Index>
    using type = std::decay_t<std::tuple_element_t<Index, base>>;

    template <size_t Index = 0>
    static bool MatchToken(Token token, const std::function<Token()>& getter) {
        if (!(type<Index>::MatchToken(token, getter)))
            return false;
        if constexpr (Index + 1 < tuple_size)
            return MatchToken<Index + 1>(getter(), getter);
        return true;
    }
};


template <typename T>
struct List {
    using type = std::decay_t<T>;

    template <size_t = 0>
    static bool MatchToken(Token token, const std::function<Token()>& getter) {
        while (type::MatchToken(token, getter)) { token = getter(); }
        return true;
    }
};

template <typename... Args>
struct RuleGroup : public std::variant<Args...> {
    using base = std::variant<Args...>;
    using base::base;
    static constexpr size_t variant_size = sizeof...(Args);
    template <size_t Index>
    using type = std::decay_t<std::variant_alternative_t<Index, base>>;

    template <size_t Index = 0>
    static bool MatchToken(Token token, const std::function<Token()>& getter) {
        if (type<Index>::MatchToken(token, getter))
            return true;
        if constexpr (Index + 1 < variant_size)
            return MatchToken<Index + 1>(getter(), getter);
        return false;
    }
};
}
}