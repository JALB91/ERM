#pragma once

namespace erm {

	class ISwapChainListener
	{
	public:
		virtual ~ISwapChainListener() = default;

		virtual void SwapChainCleanup() = 0;
		virtual void SwapChainCreated() = 0;
	};

} // namespace erm
