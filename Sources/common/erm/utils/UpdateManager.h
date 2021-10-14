#pragma once

#include "erm/utils/IUpdatable.h"

#include <vector>

namespace erm {

class UpdateManager
{
public:
	UpdateManager();
	~UpdateManager();

	void AddUpdatable(IUpdatable* updatable);
	void RemoveUpdatable(IUpdatable* updatable);

	void OnPreUpdate();
	void Update(float dt);
	void OnPostUpdate();

private:
	void Refresh();
	void ForEach(const std::function<void(IUpdatable*)>& func);

	std::vector<IUpdatable*> mUpdatables;
	std::vector<IUpdatable*> mToAdd;
	bool mIterating;
};

} // namespace erm
