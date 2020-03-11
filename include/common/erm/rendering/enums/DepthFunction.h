#pragma once

namespace erm {

	enum class DepthFunction : short
	{
		ALWAYS,
		NEVER,
		LESS,
		EQUAL,
		LEQUAL,
		GREATER,
		NOT_EQUAL,
		GEQUAL
	};

}