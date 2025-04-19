#pragma once

namespace erm::concepts {

template<typename T>
concept Module = requires(T type) {
	T::kDependencies;
	{ type.init() } -> std::same_as<void>;
	{ type.visited() } -> std::same_as<bool>;
	{ type.markVisited() } -> std::same_as<void>;
	{ type.initialized() } -> std::same_as<bool>;
	{ type.markInitialized() } -> std::same_as<void>;
};

}