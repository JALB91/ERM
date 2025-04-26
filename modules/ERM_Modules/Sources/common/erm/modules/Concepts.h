#pragma once

namespace erm::concepts {

template<typename T>
concept Module = requires(T type) {
	T::kDependencies;
	{ type.init() } -> std::same_as<bool>;
	{ type.deinit() } -> std::same_as<bool>;
	{ type.initialized() } -> std::same_as<bool>;
};

template<typename T>
concept MainModule = requires(T type, int argc, char** argv) {
	{ type.run(argc, argv) } -> std::same_as<int>;
};

}