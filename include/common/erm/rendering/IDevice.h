#pragma once

namespace erm {

	class IDevice
	{
	public:
		virtual ~IDevice() = default;

		virtual void OnRender() = 0;
	};

} // namespace erm
