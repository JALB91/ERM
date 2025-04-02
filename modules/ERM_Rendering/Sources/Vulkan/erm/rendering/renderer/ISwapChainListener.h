#pragma once

namespace erm {

class ISwapChainListener
{
public:
	virtual ~ISwapChainListener() = default;

	virtual void swapChainCleanup() = 0;
	virtual void swapChainCreated() = 0;
};

} // namespace erm
