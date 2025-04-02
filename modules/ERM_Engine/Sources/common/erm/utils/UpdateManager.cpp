#include "erm/utils/UpdateManager.h"

#include "erm/utils/Profiler.h"

namespace erm {

UpdateManager::UpdateManager()
	: mIterating(false)
{}

UpdateManager::~UpdateManager()
{}

void UpdateManager::addUpdatable(IUpdatable* updatable)
{
	if (!updatable || std::find(mUpdatables.begin(), mUpdatables.end(), updatable) != mUpdatables.end())
	{
		return;
	}

	if (mIterating)
	{
		mToAdd.emplace_back(updatable);
		return;
	}

	mUpdatables.emplace_back(updatable);
}

void UpdateManager::removeUpdatable(IUpdatable* updatable)
{
	auto it = std::find(mUpdatables.begin(), mUpdatables.end(), updatable);
	if (!updatable || it == mUpdatables.end())
	{
		return;
	}

	if (mIterating)
	{
		*it = nullptr;
		return;
	}

	mUpdatables.erase(it);
}

void UpdateManager::refresh()
{
	for (auto updatable : mToAdd)
	{
		addUpdatable(updatable);
	}

	mToAdd.clear();

	for (auto it = mUpdatables.begin(); it != mUpdatables.end();)
	{
		if (*it == nullptr)
		{
			it = mUpdatables.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void UpdateManager::forEach(const std::function<void(IUpdatable*)>& func)
{
	mIterating = true;
	for (auto& updatable : mUpdatables)
	{
		if (updatable)
		{
			func(updatable);
		}
	}
	mIterating = false;
}

void UpdateManager::preUpdate()
{
	ERM_PROFILE_FUNCTION();
	refresh();
	forEach([](IUpdatable* updatable) {
		updatable->preUpdate();
	});
}

void UpdateManager::update(float dt)
{
	ERM_PROFILE_FUNCTION();
	forEach([dt](IUpdatable* updatable) {
		updatable->update(dt);
	});
}

void UpdateManager::postUpdate()
{
	ERM_PROFILE_FUNCTION();
	forEach([](IUpdatable* updatable) {
		updatable->postUpdate();
	});
}

} // namespace erm
