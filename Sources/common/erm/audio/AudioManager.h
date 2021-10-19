#pragma once

#include "erm/audio/AudioDriver.h"
#include "erm/audio/Channel.h"
#include "erm/audio/Reproduction.h"
#include "erm/audio/Sound.h"

#include "erm/utils/IUpdatable.h"

#include <array>
#include <map>
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

	void Suspend();
	void Resume();

	void OnUpdate(float dt);
	void UpdateDrivers();

	int GetNumDrivers() const;

	void SetDriver(int driver) const;
	int GetDriver() const;

	void SetPlayInBackground(bool playInBackground);
	bool ShouldPlayInBackground() const;

	Sound* GetSound(const char* path, bool create = false);
	Reproduction* PlaySound(const char* path);
	Reproduction* PlaySound(Sound& sound);

	inline const auto& GetDrivers() const { return mDrivers; }
	inline const auto& GetSounds() const { return mSounds; }
	inline const auto& GetChannels() const { return mChannels; }
	inline const auto& GetReproductions() const { return mReproductions; }

private:
	Channel* GetFreeChannel();

	FMOD::Studio::System* mStudioSystem;
	FMOD::System* mCoreSystem;
	std::map<int, AudioDriver> mDrivers;
	std::vector<Sound> mSounds;
	std::array<Channel, MAX_CHANNELS> mChannels;
	std::vector<Reproduction> mReproductions;
	std::vector<std::reference_wrapper<Reproduction>> mReproductionsToResume;
	bool mPlayInBackground;
};

} // namespace erm
