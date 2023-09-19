#pragma once

#include <erm/utils/StaticString.h>

#include <string_view>

namespace FMOD {
class Sound;
}

namespace erm {

class Sound
{
public:
	friend class AudioManager;

public:
	Sound(
		std::string_view path,
		FMOD::Sound* sound,
		bool isStream);
	virtual ~Sound();

	inline const str256& getPath() const { return mPath; }
	inline bool isStream() const { return mIsStream; }

	str256 getName() const;
	unsigned int getLenght() const;

protected:
	str256 mPath;
	FMOD::Sound* mSound;
	bool mIsStream;
};

} // namespace erm
