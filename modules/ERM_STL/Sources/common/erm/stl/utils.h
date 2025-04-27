#pragma once

#include <refl.hpp>

namespace erm::stl {

template<typename Container, typename T>
bool contains(const Container& container, T value)
{
	return std::find(container.cbegin(), container.cend(), value) != container.cend();
}

template<typename Container, typename T>
auto find_or(
	const Container& container, 
	typename Container::key_type key, 
	T fallback)
{
	const auto it = container.find(key);
	return it == container.end() ? std::move(fallback) : it->second;
}

namespace internal {

template<typename F, typename T>
constexpr auto invoke_optional_index(F f, size_t idx, int) -> decltype(f.template operator()<T>(idx))
{
	return f.template operator()<T>(idx);
}

template<typename F, typename T>
constexpr auto invoke_optional_index(F f, size_t, ...) -> decltype(f.template operator()<T>())
{
	return f.template operator()<T>();
}

template<typename F>
constexpr void eval_in_order(refl::type_list<>, std::index_sequence<>, [[maybe_unused]] F f)
{
}

template<typename F, typename T, typename... Ts, size_t I, size_t... Idx>
constexpr void eval_in_order(refl::type_list<T, Ts...>, std::index_sequence<I, Idx...>, F f)
{
	invoke_optional_index<F, T>(f, I, 0);
	return eval_in_order(
		refl::type_list<Ts...> {},
		std::index_sequence<Idx...> {},
		std::forward<F>(f));
}

}

template<typename F, typename... Ts>
constexpr void for_each_type(refl::type_list<Ts...> list, F&& f)
{
	internal::eval_in_order(list, std::make_index_sequence<sizeof...(Ts)> {}, std::forward<F>(f));
}

}