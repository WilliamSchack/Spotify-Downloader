#ifndef VARIANTUTILS_H
#define VARIANTUTILS_H

// For std::visit, from the cpp docs https://en.cppreference.com/cpp/utility/variant/visit2
template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

#endif