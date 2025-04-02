#pragma once

#include "erm/audio/AudioDriver.h"
#include "erm/audio/Channel.h"
#include "erm/audio/Reproduction.h"
#include "erm/audio/Sound.h"

#include <array>
#include <string_view>
#include <unordered_map>
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

	void suspend();
	void resume();

	void update(float dt);
	void updateDrivers();

	int getNumDrivers() const;

	void setDriver(int driver) const;
	int getDriver() const;

	void setPlayInBackground(bool playInBackground);
	bool shouldPlayInBackground() const;

	Sound* getSound(std::string_view path, bool create = false);
	Reproduction* playSound(std::string_view path);
	Reproduction* playSound(Sound& sound);

	inline const auto& getDrivers() const { return mDrivers; }
	inline const auto& getSounds() const { return mSounds; }
	inline const auto& getChannels() const { return mChannels; }
	inline const auto& getReproductions() const { return mReproductions; }

private:
	Channel* getFreeChannel();

	FMOD::Studio::System* mStudioSystem;
	FMOD::System* mCoreSystem;
	std::unordered_map<int, AudioDriver> mDrivers;
	std::vector<Sound> mSounds;
	std::array<Channel, MAX_CHANNELS> mChannels;
	std::vector<Reproduction> mReproductions;
	std::vector<std::reference_wrapper<Reproduction>> mReproductionsToResume;
	bool mPlayInBackground;
	bool mSuspended;
};

} // namespace erm
