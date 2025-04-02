#pragma once

namespace erm {

class IUpdatable
{
public:
	IUpdatable();
	virtual ~IUpdatable();

	virtual void preUpdate() {}
	virtual void update(float /*dt*/) {}
	virtual void postUpdate() {}
};

} // namespace erm
