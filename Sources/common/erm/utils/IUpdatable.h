#pragma once

namespace erm {

class IUpdatable
{
public:
	IUpdatable();
	virtual ~IUpdatable();

	virtual void OnPreUpdate() {}
	virtual void OnUpdate(float /*dt*/) {}
	virtual void OnPostUpdate() {}
};

} // namespace erm
