#pragma once

#include "erm/audio/Channel.h"
#include "erm/audio/Reproduction.h"
#include "erm/audio/Sound.h"

#include "erm/utils/IUpdatable.h"

#include <array>
#include <vector>

namespace FMOD {
class System;
namespace Studio {
class System;
}
} // namespace FMOD

namespace erm {

class AudioManager
{
public:
	static constexpr int MAX_CHANNELS = 512;

public:
	AudioManager();
	~AudioManager();

	void OnUpdate(float dt);

	Sound* GetSound(const char* path, bool create = false);
	Reproduction* PlaySound(const char* path);
	Reproduction* PlaySound(Sound& sound);

	inline const auto& GetSounds() const { return mSounds; }
	inline const auto& GetChannels() const { return mChannels; }
	inline const auto& GetReproductions() const { return mReproductions; }

private:
	Channel* GetFreeChannel();

	FMOD::System* mCoreSystem;
	FMOD::Studio::System* mStudioSystem;
	std::vector<Sound> mSounds;
	std::array<Channel, MAX_CHANNELS> mChannels;
	std::vector<Reproduction> mReproductions;
};

} // namespace erm
