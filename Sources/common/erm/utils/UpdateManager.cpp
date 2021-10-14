#include "erm/utils/UpdateManager.h"

#include "erm/utils/Profiler.h"

namespace erm {

UpdateManager::UpdateManager()
	: mIterating(false)
{}

UpdateManager::~UpdateManager()
{}

void UpdateManager::AddUpdatable(IUpdatable* updatable)
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

void UpdateManager::RemoveUpdatable(IUpdatable* updatable)
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

void UpdateManager::Refresh()
{
	for (auto updatable : mToAdd)
		AddUpdatable(updatable);

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

void UpdateManager::ForEach(const std::function<void(IUpdatable*)>& func)
{
	mIterating = true;
	for (auto& updatable : mUpdatables)
		if (updatable)
			func(updatable);
	mIterating = false;
}

void UpdateManager::OnPreUpdate()
{
	PROFILE_FUNCTION();
	Refresh();
	ForEach([](IUpdatable* updatable) {
		updatable->OnPreUpdate();
	});
}

void UpdateManager::Update(float dt)
{
	PROFILE_FUNCTION();
	ForEach([dt](IUpdatable* updatable) {
		updatable->OnUpdate(dt);
	});
}

void UpdateManager::OnPostUpdate()
{
	PROFILE_FUNCTION();
	ForEach([](IUpdatable* updatable) {
		updatable->OnPostUpdate();
	});
}

} // namespace erm
