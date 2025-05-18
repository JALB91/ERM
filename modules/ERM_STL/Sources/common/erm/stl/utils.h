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
constexpr auto eval_in_order(refl::type_list<>, std::index_sequence<>, [[maybe_unused]] F f)
{
}

template<typename F, typename T, typename... Ts, size_t I, size_t... Idx>
constexpr auto eval_in_order(refl::type_list<T, Ts...>, std::index_sequence<I, Idx...>, F f)
{
	invoke_optional_index<F, T>(f, I, 0);
	return eval_in_order(
		refl::type_list<Ts...> {},
		std::index_sequence<Idx...> {},
		std::forward<F>(f));
}

}

template<typename F, typename... Ts>
constexpr auto for_each_type(refl::type_list<Ts...> list, F&& f)
{
	return internal::eval_in_order(list, std::make_index_sequence<sizeof...(Ts)> {}, std::forward<F>(f));
}

namespace internal {

template<typename, bool...>
struct apply_mask;

template<>
struct apply_mask<refl::type_list<>>
{
	using type = refl::type_list<>;
};

template<typename T, typename... Ts, bool... Bs>
struct apply_mask<refl::type_list<T, Ts...>, true, Bs...>
{
	using type = refl::trait::prepend_t<T, typename apply_mask<refl::type_list<Ts...>, Bs...>::type>;
};

template<typename T, typename... Ts, bool... Bs>
struct apply_mask<refl::type_list<T, Ts...>, false, Bs...> : apply_mask<refl::type_list<Ts...>, Bs...>
{};

template<typename F, typename... Ts>
constexpr auto filter([[maybe_unused]] F f, refl::type_list<Ts...>)
{
	return typename apply_mask<refl::type_list<Ts...>, f.template operator()<Ts>()...>::type {};
}

} // namespace internal

/**
         * Filters the list according to a *constexpr* predicate.
         * Calling f(Ts{})... should be valid in a constexpr context.
         *
         * \code{.cpp}
         * filter(reflect_types(type_list<int, long, float>{}), [](auto td) {
         *   return std::is_integral_v<typename decltype(td)::type>;
         * })
         *   -> type_list<type_descriptor<int>, type_descriptor<long>>
         * \endcode
         */
template<typename F, typename... Ts>
constexpr auto filter_type(refl::type_list<Ts...> list, F&& f)
{
	return decltype(internal::filter(std::forward<F>(f), list))();
}

/*
* Returns the initial_value unchanged.
*/
/// \private
template<typename R, typename F, typename... Ts>
constexpr R accumulate_type(refl::type_list<>, R&& initial_value, F&&)
{
	return std::forward<R>(initial_value);
}

/*
* Applies an accumulation function F to each type in the refl::type_list.
*/
template<typename R, typename F, typename T, typename... Ts>
constexpr auto accumulate_type(refl::type_list<T, Ts...> ts, R&& initial_value, F&& f)
{
	return accumulate_type(
		refl::type_list<Ts...> {}, 
		f.template operator()<T>(std::forward<R>(initial_value)),
		std::forward<F>(f));
}

}