#pragma once

#include "erm/utils/IUpdatable.h"

#include <functional>
#include <vector>

namespace erm {

class UpdateManager
{
public:
	UpdateManager();
	~UpdateManager();

	void addUpdatable(IUpdatable* updatable);
	void removeUpdatable(IUpdatable* updatable);

	void preUpdate();
	void update(float dt);
	void postUpdate();

private:
	void refresh();
	void forEach(const std::function<void(IUpdatable*)>& func);

	std::vector<IUpdatable*> mUpdatables;
	std::vector<IUpdatable*> mToAdd;
	bool mIterating;
};

} // namespace erm
